pluginManagement {
    repositories {
        google()
        mavenCentral()
        gradlePluginPortal()
    }
}
dependencyResolutionManagement {
    repositoriesMode.set(RepositoriesMode.FAIL_ON_PROJECT_REPOS)
    repositories {
        google()
        mavenCentral()
        // Add the following line if not present
        maven { url = uri ("https://dl.google.com/android/maven2") }
        maven { url = uri ("https://jitpack.io") }
    }
}



rootProject.name = "Cosmo388"
include(":app")
