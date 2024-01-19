package com.example.cosmo388

import android.content.pm.PackageManager
import android.os.Build
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.util.Log
import android.widget.Button
import androidx.core.content.ContextCompat
import android.Manifest
import android.content.Context
import android.content.res.Resources
import android.net.Uri
import android.os.Environment
import androidx.core.app.ActivityCompat
import android.widget.Toast
import androidx.recyclerview.widget.LinearLayoutManager
import androidx.recyclerview.widget.RecyclerView
import com.example.oboe.R
import java.io.File
import java.io.FileNotFoundException
import java.io.FileOutputStream
import java.io.IOException
import java.text.SimpleDateFormat
import java.util.Date
import java.util.Locale

private const val TAG = "Main Activity"
class MainActivity : AppCompatActivity()
{

    //CPP SETUP AND LOADERS
    private var recorderHandle: Long = 0
    private external fun createAudioRecorder(): Long
    private external fun startRecording(recorderHandle: Long)
    private external fun stopRecording(recorderHandle: Long)
    private external fun startPlayback(recorderHandle: Long)
    private val RECORD_AUDIO_PERMISSION_CODE = 1
    private external fun getPath(storagePath: String, recorderHandle: Long)
    private external fun writeAudio(date: String, recorderHandle: Long)
    private external fun getIRPath(irPath: String, recorderHandle: Long)



    companion object
    {
        // Load the native library containing the JNI functions
        init
        {
            Log.i("MainActivity", "Native library loaded")
            System.loadLibrary("oboe-lib")
        }
    }

    private fun getCurrentTimeDate() : String
    {
        val dateFormat = SimpleDateFormat("yyyy-MM-dd HH:mm:ss", Locale.getDefault())
        val currentDate = Date();
        return dateFormat.format(currentDate)
    }



    override fun onCreate(savedInstanceState: Bundle?)
    {

        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        //Check if we have the permissions for Audio Handling
        checkRecordAudioPermission()

        //Declare UI variables
        val startButton: Button = findViewById(R.id.bt_StartButton);
        val stopButton: Button = findViewById(R.id.bt_stopButton);
        val playbackButton: Button = findViewById(R.id.bt_PlaybackButton)
        val saveButton: Button = findViewById(R.id.bt_SaveRecording)
        val systemMenu : RecyclerView = findViewById(R.id.rv_systemMenu)

        // Fetch the IR resource
        val resources: Resources = resources
        //.wav ?
        val fileName = "ir"
        val resID = resources.getIdentifier(fileName, "raw", packageName)

// Open the resource input stream
        val inputStream = resources.openRawResource(resID)

// Create a FileOutputStream for the internal storage
        val outputStream: FileOutputStream
        try {
            outputStream = openFileOutput(fileName, Context.MODE_PRIVATE)
        } catch (e: FileNotFoundException) {
            Log.e(TAG, "File not found: $e")
            return
        }

// Copy the contents of the input stream to the output stream
        val buffer = ByteArray(1024)
        var length: Int
        try {
            while (inputStream.read(buffer).also { length = it } > 0) {
                outputStream.write(buffer, 0, length)
            }
        } catch (e: IOException) {
            Log.e(TAG, "Error copying file: $e")
        } finally {
            // Close streams
            inputStream.close()
            outputStream.close()
        }

// Get the internal storage file path
        val internalFilePath = File(filesDir, fileName).absolutePath

// Initialize recorder
        recorderHandle = createAudioRecorder()

// Pass the IR file path to the AudioRecorder Class
        getIRPath(internalFilePath, recorderHandle)
        Log.i(TAG, "IR File Path: $internalFilePath")




        //Fetch where we will be saving our audio files
        var storagePath = getExternalFilesDir(null)?.absolutePath
        getPath(storagePath.toString(), recorderHandle)



        Log.i(TAG, "PRE Storage Path  =  $storagePath")

        //solution:
      //  storagePath = "/storage/emulated/0/Android/data/com.example.oboe/"

        //assign adapter
        val adapter = FileListAdapter(mutableListOf())
        systemMenu.adapter = adapter

        // Set the layout manager (vertical in this case)
        systemMenu.layoutManager = LinearLayoutManager(this)
        updateUIWithFiles(this ,storagePath.toString(), systemMenu, adapter )


        startButton.setOnClickListener {
            Log.i(TAG, "Record clicked")
            startRecording(recorderHandle)
        }

        stopButton.setOnClickListener {
            Log.i(TAG, "Stop clicked")
            stopRecording(recorderHandle)
        }

        playbackButton.setOnClickListener(){
            Log.i(TAG, "PlayBack Clicked!")
            startPlayback(recorderHandle)
        }

        saveButton.setOnClickListener()
        {
            Log.i(TAG, "Save Button Clicked")
            writeAudio(getCurrentTimeDate(), recorderHandle)
        }





    }

    private fun updateUIWithFiles(context: Context, folderPath: String, recyclerView: RecyclerView, adapter: FileListAdapter)
    {
        val directory = File(folderPath)
        Log.i(TAG, "Folderpath is: $folderPath")
        if (directory.exists() && directory.isDirectory)
        {
            val fileNames = directory.listFiles()?.map { it.name } ?: emptyList()

            // Log the file names for debugging
            Log.i(TAG, "File list in directory: $fileNames")

            // Convert file names to a list of FileName objects
            val fileNameObjects = fileNames.map { FileName(it) }

            // Update the adapter with the new data
            adapter.updateData(fileNameObjects)
        }
        else
        {
           Log.i(TAG, "Folder Path does not exist")
        }
    }


    private fun checkRecordAudioPermission()
    {
        // Check if the permission is not granted
        if (ContextCompat.checkSelfPermission(
                this,
                Manifest.permission.RECORD_AUDIO

            ) != PackageManager.PERMISSION_GRANTED
        ) {
            // Request the permission
            ActivityCompat.requestPermissions(
                this,
                arrayOf(Manifest.permission.RECORD_AUDIO),
                RECORD_AUDIO_PERMISSION_CODE
            )
        }
    }

    // Handle the result of the permission request
    override fun onRequestPermissionsResult(
        requestCode: Int,
        permissions: Array<out String>,
        grantResults: IntArray
    ) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults)
        when (requestCode) {
            RECORD_AUDIO_PERMISSION_CODE -> {
                // Check if the permission is granted
                if (grantResults.isNotEmpty() && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                    // Permission granted, proceed with your audio-related tasks
                }
                else
                {
                    Toast.makeText(this,"Audio Device Permission not Granted, app will not work", Toast.LENGTH_LONG)
                }
            }

        }
    }


}