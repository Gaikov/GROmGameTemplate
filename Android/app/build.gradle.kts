
import java.util.Properties

plugins {
    alias(libs.plugins.androidApplication)
}

val keystoreProperties = Properties().apply {
    val propertiesFile = rootProject.file("keystore.properties")
    if (propertiesFile.isFile) {
        propertiesFile.inputStream().use(::load)
    }
}

fun signingValue(propertyName: String, environmentName: String): String? =
    keystoreProperties.getProperty(propertyName) ?: System.getenv(environmentName)

val releaseStoreFile = signingValue("storeFile", "GROM_ANDROID_STORE_FILE")
val releaseStorePassword = signingValue("storePassword", "GROM_ANDROID_STORE_PASSWORD")
val releaseKeyAlias = signingValue("keyAlias", "GROM_ANDROID_KEY_ALIAS")
val releaseKeyPassword = signingValue("keyPassword", "GROM_ANDROID_KEY_PASSWORD")
val hasReleaseSigning = listOf(
    releaseStoreFile,
    releaseStorePassword,
    releaseKeyAlias,
    releaseKeyPassword
).all { !it.isNullOrBlank() }

android {
    ndkVersion = "29.0.14206865"

    signingConfigs {
        create("release") {
            if (hasReleaseSigning) {
                storeFile = file(requireNotNull(releaseStoreFile))
                storePassword = releaseStorePassword
                keyAlias = releaseKeyAlias
                keyPassword = releaseKeyPassword
            }
        }
    }
    namespace = "com.grom.template"
    compileSdk = 36

/*    androidResources {
        noCompress.add("assets.pak")
    }*/

    sourceSets.getByName("main") {
        assets.srcDirs("../../assets")
    }

    androidResources {
        ignoreAssetsPatterns.addAll(
            listOf(
                "!.svn",
                "!.git",
                "!.DS_Store",
                "!*.scc",
                ".*",
                "<dir>_*",
                "!CVS",
                "!thumbs.db",
                "!picasa.ini",
                "!*~",
                "!libEGL.dylib",
                "!libGLESv2.dylib",
                "!*.log",
                "<dir>metal"
            )
        )
    }

    defaultConfig {
        applicationId = "com.grom.template"
        minSdk = 23
        targetSdk = 36
        versionCode = 1
        versionName = "1.0.0.beta.1"

        testInstrumentationRunner = "androidx.test.runner.AndroidJUnitRunner"
        externalNativeBuild {
            cmake {
                cppFlags += "-std=c++20"
                arguments += "-DANDROID_STL=c++_shared"
            }
        }
    }

    buildTypes {
        release {
            isMinifyEnabled = false
            proguardFiles(getDefaultProguardFile("proguard-android-optimize.txt"), "proguard-rules.pro")
            ndk.debugSymbolLevel = "FULL"
            if (hasReleaseSigning) {
                signingConfig = signingConfigs.getByName("release")
            }
        }
    }
    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_17
        targetCompatibility = JavaVersion.VERSION_17
    }
    buildFeatures {
        prefab = true
    }
    externalNativeBuild {
        cmake {
            path = file("../../CMakeLists.txt")
            version = "3.22.1"
        }
    }
}

dependencies {
    implementation(libs.appcompat)
    implementation(libs.core)
    implementation(libs.games.activity)
    implementation(libs.games.frame.pacing)
    testImplementation(libs.junit)
    androidTestImplementation(libs.ext.junit)
    androidTestImplementation(libs.espresso.core)

}
