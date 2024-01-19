
#include <iostream>
#include <vector>
#include <oboe/Oboe.h>
#include <android/log.h>
#include "Audiofile.h"
#include <random>

#include "FFTConvolver.h"
#include "AudioFFT.h"
#include "Utilities.h"

#define LOG_TAG "MyApp"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

/* AudioStreamBuilder -> AudioStream ( I/O ) -> AudioStreamCallack (Auio Processor )
 *
 * Oboe, you use the AudioStreamBuilder to set up the desired audio parameters, and then you pass it to the AudioStream object, which manages the I/O operations.
 * The AudioStreamCallback is used to process the audio data during I/O operations. Here's a breakdown of the flow:

    AudioStreamBuilder: Used to configure the audio stream parameters such as direction (input or output), performance mode, sharing mode, format, channel count, etc.

    AudioStream: Created from the AudioStreamBuilder. Manages the audio I/O operations (reading or writing audio data).

    AudioStreamCallback: Implemented by the user to process audio data. The onAudioReady method is called when the audio data is ready for processing.
    You can perform any audio processing or analysis in this callback.

    Audio Stream Lifecycle: The AudioStream is responsible for opening, starting, stopping, and closing the audio stream.

    It ensures that the audio data is delivered to the callback at the appropriate times.*/
enum streamState
{
    playback,
    record
};

class AudioRecorder : public oboe::AudioStreamCallback{
public:

    AudioRecorder()
    {
        LOGI("Audio Recorder Object Created");
    }

    void setSystemPath(const std::string &filePath)
    {
        LOGI("ANDROID FILEPATH %s ", filePath.c_str());
        systemPath = filePath;
    }

    void callPlayBackStream()
    {
        try
        {
            StreamState = playback;
            streamIntializer();
            AudioFile<float> loadFile;
            if (loadFile.load(playbackPath))
            {
               /* auto bufferSize = std::min(loadFile.samples[0].size(), audioDataBuffer.size());
                std::copy(loadFile.samples[0].begin(), loadFile.samples[0].begin() + bufferSize, audioDataBuffer.begin());*/
                //   //assumes MONO
                std::copy(loadFile.samples[0].begin(), loadFile.samples[0].end(), audioDataBuffer.begin());
                LOGI("Requesting to Start PLAYBACK Stream");
                oboe::Result result = audioStream->requestStart();
                if (result != oboe::Result::OK)
                {
                    throw std::runtime_error("ERROR STARTING PLAYBACK Stream");
                }
            }
            else
            {
                throw std::runtime_error("ERROR LOADING FILE");
            }
        }
        catch (std::exception &e)
        {
            LOGI("EXCEPTION THROWN: %s", e.what());
        }
    }

    void callRecorderStream()
    {
        try
        {
            StreamState = record;
            streamIntializer();
            LOGI("Requesting to Start Record Stream");
            oboe::Result result = audioStream->requestStart();
            if (result != oboe::Result::OK)
            {
                throw std::runtime_error("ERROR STARTING RECORD Stream");
            }
        }

        catch (std::exception &e)
        {
            LOGI("EXCEPTION THROWN: %s", e.what());
        }
    }

    void stopAllStream()
    {
        if (audioStream)
        {
            audioStream->requestStop();
            audioStream->close();
        }
        else
            LOGI("INFO: NO STREAM TO STOP!");
    }

    void writeAudio(const std::string& date)
    {
        LOGI("end buffer size: %i", audioDataBuffer.size() );
        //audiofile is a vector of vectors
        AudioFile<float>::AudioBuffer writebuffer;
        //we want one vector (mono)
        writebuffer.resize(1);
        //make that vector the size of the audiobuffer
        writebuffer[0].resize(audioDataBuffer.size());
        //copy the contents of one to another
        std::copy(audioDataBuffer.begin(), audioDataBuffer.end(), writebuffer[0].begin());

        try
        {
            //LOGI("storage path: %s", systemPath.c_str());
            finalPath = systemPath + "/" + date + ".wav";
            LOGI("final path: %s", finalPath.c_str());

            //write that file to the system path provided by android
            AudioFile<float> fileWrite;
            fileWrite.setAudioBuffer(writebuffer);
            if(fileWrite.save(finalPath))
            {
                LOGI("Write Sucessful TO: %s", finalPath.c_str());

            }
            else
            {
                std::string error = "Error on write: " + finalPath;
                throw std::runtime_error(error);

            }
        }
        catch(std::exception& e)
        {
            LOGI("ERROR: %s", e.what());
        }
        convolution();

    }

    void getIRPath(std::string path)
    {
        irPath = path;

    }

    std::vector<float> WOW_and_FLUTTER_Function(const std::vector<float>& input)
    {
        const double modfreq = 0.7 / 44100;
        const double preCalWave = modfreq * 2 * pi;
        const int width = static_cast<int>(std::round(0.002 * 44100));
        const int L = 2 + width + width * 2;
        const int len = input.size();
        int writeIndex = 0;
        double mod = 0.0, tap = 0.0, frac = 0.0;
        int i = 0, idx = 0;

        std::vector<float> y(len, 0.0);
        std::vector<double> Delayline(L, 0.0);

        for (int n = 0; n < len; n++)
        {
            mod = std::sin(preCalWave * n);
            tap = 1 + width + width * mod;
            i = static_cast<int>(tap);
            frac = tap - i;

            Delayline[writeIndex] = input[n];
            writeIndex = (writeIndex + 1) % L;

            idx = (writeIndex + L - i - 1) % L;

            // Linear Interpolation
            y[n] = Delayline[idx] * frac + Delayline[(idx + 1) % L] * (1 - frac);
        }

        return y;
    }


    void convolution()
    {
        //possible problemo
        fftconvolver::FFTConvolver fft;
        std::string Input_filename = finalPath;

        const std::string Impulse_filename = irPath;
        const std::string outputfilename = systemPath + "/" + "Processed.wav";

        // Open the audio file
        AudioFile<float> inputFile;
        AudioFile<float> impulsefile;

        try
        {
            std::string error;

            if (!inputFile.load(Input_filename))
            {
                error = "Failed to open input audio file:  " + Input_filename;
                throw std::runtime_error(error);
            }
            else
            {
                LOGI("CONVOLUTION: Input File Loaded");
            }

            if (!impulsefile.load(Impulse_filename))
            {
                error = "Failed to open impulse audio file";
                LOGI("IR LOCATION : %s", irPath.c_str());
                throw std::runtime_error(error);
            }
            else
            {
                LOGI("Impulse Response Loaded");
            }
        }
        catch (std::exception& e)
        {
            LOGI("Convolution Load ERROR:: %s", e.what());
            return;
        }

        int inputtotalsampleSize = inputFile.getNumSamplesPerChannel();
        int impulsetotalsampleSize = impulsefile.getNumSamplesPerChannel();

        /***************************************************************************************STEP 1: Convolve Audio Signal***************************************************************************************************/

        // Prepare input
        std::vector<fftconvolver::Sample>& in = inputFile.samples[0];

        // Prepare Impulse
        std::vector<fftconvolver::Sample>& ir = impulsefile.samples[0];

        // Prepare Ouput
        std::vector<fftconvolver::Sample> out(inputFile.getNumSamplesPerChannel());

        // Perform Convolution
        if (fft.init(1024, &ir[0], impulsetotalsampleSize))
        {
            LOGI("Convolution PROCESSING");
            fft.process(&in[0], &out[0], inputtotalsampleSize);
        }
        else
        {
            LOGI("FAILED TO INTIALIZE AND PERFORM CONVOLUTION");
        }

        LOGI("Main Audio Convolution Ended ");

        /****************************************************************************** STEP 2: Combine White Noise with Convolved Signal ***************************************************************************************/

        // Generate white noise
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
        std::vector<float> whiteNoise(inputtotalsampleSize);
        const float noiseLevel = -56.0f;                                                // Desired noise level in dB -56 natively
        const float noiseAmplitude = std::pow(10.0f, noiseLevel / 20.0f);

        LOGI("WHITE NOISE PROCESSING INTIATED");

        for (size_t i = 0; i < inputtotalsampleSize; i++)
        {
            whiteNoise[i] = dist(gen) * noiseAmplitude;
        }

        LOGI("WHITE NOISE PROCESSING ENDED");

        //Convolve the White Noise
        std::vector<float> noiseConvolved(inputtotalsampleSize);
        fft.process(&whiteNoise[0], &noiseConvolved[0], inputtotalsampleSize);

        // Combine the output and the noise-convolved output into the same buffer (combinedSignal) and Normalize the output
        std::vector<float> combinedSignal(inputtotalsampleSize);
        combinedSignal.reserve(inputtotalsampleSize);
        const float targetLevel = -12.0f; // Target level in dB
        const float normalizationFactor = std::pow(10.0f, targetLevel / 20.0f);
        for (size_t i = 0; i < inputtotalsampleSize; i++)
        {
            combinedSignal[i] = out[i] + noiseConvolved[i];
            combinedSignal[i] *= normalizationFactor;
        }

        /****************************************************************************** STEP 3: WOW AND FLUTTER ***************************************************************************************/


        std::vector<float> finalSignal = WOW_and_FLUTTER_Function(combinedSignal);




        /****************************************************************************** STEP 4: Export the Audio File ***************************************************************************************/


        // Create a multi-channel buffer with a single channel
        AudioFile<float>::AudioBuffer buffer;
        buffer.resize(1);
        buffer[0] = std::move(finalSignal);


        // Create an AudioFile object
        AudioFile<float> audioFile;
        audioFile.setAudioBuffer(buffer);

        // Set the sample rate and number of channels
        audioFile.setSampleRate(44100);
        audioFile.setNumChannels(1); // Mono audio

        // Write the audio data to the WAV file
        if (!audioFile.save(outputfilename))
        {
            LOGI("Failed to write audio file");
            return;
        }


       LOGI("CONVOLUTE OUTPUT file saved: %s", outputfilename.c_str());


    }

    ~AudioRecorder()
    {
        // Add any necessary cleanup logic
    }

private:
    streamState StreamState;
    std::vector<float> audioDataBuffer;
    const int sampleWindow = 1024; // Adjust as needed
    std::shared_ptr<oboe::AudioStream> audioStream;
    std::string systemPath;
    std::string playbackPath;
    std::string irPath;
    const double pi = 3.14159265358979323846;
    std::string finalPath;


    void streamIntializer()
    {
        if (audioStream)
        {
            audioStream->requestStop();
            audioStream->close();
        }

        if (StreamState == playback)
        {
            oboe::AudioStreamBuilder builder;
            builder.setCallback(this);
            builder.setDirection(oboe::Direction::Output);
            builder.setPerformanceMode(oboe::PerformanceMode::LowLatency);
            builder.setFormat(oboe::AudioFormat::Float);
            builder.setSampleRate(44100);
            builder.setChannelCount(oboe::ChannelCount::Mono);

            LOGI("Preparing to Open playback Stream");
            oboe::Result result = builder.openStream(audioStream);
            if (result != oboe::Result::OK)
            {
                throw std::runtime_error("ERROR: UNABLE TO OPEN PLAYBACK STREAM");
            }

        }

        else if (StreamState == record)
        {
            oboe::AudioStreamBuilder builder;
            builder.setCallback(this);
            builder.setDirection(oboe::Direction::Input);
            builder.setPerformanceMode(oboe::PerformanceMode::LowLatency);
            builder.setFormat(oboe::AudioFormat::Float);
            builder.setSampleRate(44100);
            builder.setChannelCount(oboe::ChannelCount::Mono);
            LOGI("Preparing to Open record Stream");
            oboe::Result result = builder.openStream(audioStream);
            if (result != oboe::Result::OK)
            {
                throw std::runtime_error("ERROR: UNABLE TO RECORD STREAM");
            }
        }
        else
        {
            throw std::runtime_error("ERROR: NO STREAM STATE PROVIDED");
        }
    }

    oboe::DataCallbackResult onAudioReady(oboe::AudioStream *audioStream, void *audioData, int32_t numFrames) override
    {
        if (StreamState == playback)
        {
            float *floatData = static_cast<float *>(audioData);
            auto bufferSize = std::min(static_cast<size_t>(numFrames), audioDataBuffer.size());
            std::copy(audioDataBuffer.begin(), audioDataBuffer.begin() + bufferSize, floatData);
        }
        else if (StreamState == record)
        {
            float *inputBuffer = static_cast<float *>(audioData);
            audioDataBuffer.insert(audioDataBuffer.end(), inputBuffer, inputBuffer + numFrames);
        }
        else
        {
            exit(1);
        }

        return oboe::DataCallbackResult::Continue;
    }
};




/*

*legacy code
 * class AudioRecorder : public oboe::AudioStreamCallback
        {
public:

    oboe::DataCallbackResult onAudioReady(oboe::AudioStream *audioStream, void *audioData, int32_t numFrames) override
    {

        //sign wave:
        float *floatData = (float *) audioData;
        for (int i = 0; i < numFrames; ++i) {
            float sampleValue = kAmplitude * sinf(mPhase);
            for (int j = 0; j < kChannelCount; j++)
            {
                floatData[i * kChannelCount + j] = sampleValue;
            }
            mPhase += mPhaseIncrement;
            if (mPhase >= kTwoPi) mPhase -= kTwoPi;
        }
        //audioDataBuffer.insert(audioDataBuffer.end(), floatData, floatData + numFrames);



    //recorder:
/*
        // Process the audio data (save it to a buffer, for example)
        //if (audioDataBuffer.size() < bufferSize)
        //{
            // Assuming Float audio format
            float *inputBuffer = static_cast<float *>(audioData);
            audioDataBuffer.insert(audioDataBuffer.end(), inputBuffer, inputBuffer + numFrames);
        //}



return oboe::DataCallbackResult::Continue;
}

void getPath(const std::string &filePath)
{
    systemPath = filePath;
    LOGI("ANDROID FILEPATH %s ", filePath.c_str() );


}

AudioRecorder()
{
    LOGI("AudioRecorder Created");
    std::cout << "Hello from cout" << std::endl;

    oboe::AudioStreamBuilder builder;

    builder.setCallback(this);
    builder.setDirection(oboe::Direction::Output);
    builder.setPerformanceMode(oboe::PerformanceMode::LowLatency);
    //builder.setAudioApi(oboe::AudioApi::AAudio);

    // builder.setSharingMode(oboe::SharingMode::Exclusive);
    builder.setFormat(oboe::AudioFormat::Float);
    builder.setSampleRate(44100);
    builder.setChannelCount(oboe::ChannelCount::Mono);

    LOGI("Preparing to OpenStream");
    oboe::Result result = builder.openStream(audioStream);
    if (result != oboe::Result::OK)
    {
        LOGI("UNABLE TO OPEN STREAM ERROR");
    }
}

~AudioRecorder()
{
    stop();
}

void writeAudio()
{

    LOGI("end buffer size: %i", audioDataBuffer.size() );
    //audiofile is a vector of vectors
    AudioFile<float>::AudioBuffer writebuffer;
    //we want one vector (mono)
    writebuffer.resize(1);
    //make that vector the size of the audiobuffer
    writebuffer[0].resize(audioDataBuffer.size());
    //copy the contents of one to another
    std::copy(audioDataBuffer.begin(), audioDataBuffer.end(), writebuffer[0].begin());

    try
    {
        std::string finalPath = systemPath +  "test.wav";
        //write that file to the system path provided by android
        AudioFile<float> fileWrite;
        fileWrite.setAudioBuffer(writebuffer);
        if(fileWrite.save(finalPath))
        {
            LOGI("Write Sucessful TO: %s", finalPath.c_str());
        }
        else
        {
            std::string error = "Error on write: " + finalPath;
            throw std::runtime_error(error);

        }
    }
    catch(std::exception& e)
    {
        LOGI("ERROR: %s", e.what());
    }

}


void start()
{
    LOGI("requesting to Start Stream");
    oboe::Result result = audioStream->requestStart();
    if (result != oboe::Result::OK) {

        // Handle error
        LOGI("Preparing to START Stream");

    }
}

void stop()
{
    if (audioStream)
    {
        audioStream->requestStop();
        audioStream->close();
        // ? . operattor refers to sharedptr function not value audioStream.reset();
        writeAudio();

    }
}


private:
std::vector<float> audioDataBuffer;
const int bufferSize = 8192; // Adjust as needed
std::shared_ptr<oboe::AudioStream> audioStream;
std::string systemPath = "";
static int constexpr kChannelCount = 1;
static int constexpr kSampleRate = 44100;
// Wave params, these could be instance variables in order to modify at runtime
static float constexpr kAmplitude = 0.5f;
static float constexpr kFrequency = 440;
static float constexpr kPI = M_PI;
static float constexpr kTwoPi = kPI * 2;
static double constexpr mPhaseIncrement = kFrequency * kTwoPi / (double) kSampleRate;
// Keeps track of where the wave is
float mPhase = 0.0;
};

* */



