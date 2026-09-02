package com.grom.template;

import android.content.ActivityNotFoundException;
import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.widget.Toast;

import androidx.appcompat.app.AlertDialog;
import androidx.core.view.WindowCompat;
import androidx.core.view.WindowInsetsCompat;
import androidx.core.view.WindowInsetsControllerCompat;

import com.google.androidgamesdk.GameActivity;

public class MainActivity extends GameActivity {
    static {
        System.loadLibrary("grom-game-mobile");
    }

    final String TAG = "GROm Native";


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        // Keep this compatibility wrapper for edge-to-edge support on Android 14 and below.
        // Google Play may attribute its internal deprecated system-bar calls to AndroidX.
        WindowCompat.enableEdgeToEdge(getWindow());
        hideSystemUi();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
    }

    public boolean showInterstitialAd() {
        return false;
    }

    public void messageBox(String caption, String message) {
        runOnUiThread(() -> {
            AlertDialog.Builder dlgAlert = new AlertDialog.Builder(this);
            dlgAlert.setMessage(message);
            dlgAlert.setTitle(caption == null || caption.isEmpty()
                    ? getString(R.string.app_name)
                    : caption);
            dlgAlert.setPositiveButton(android.R.string.ok, null);
            dlgAlert.setCancelable(true);
            dlgAlert.show();
        });
    }

    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);

        if (hasFocus) {
            hideSystemUi();
        }
    }

    private void hideSystemUi() {
        WindowInsetsControllerCompat controller =
                WindowCompat.getInsetsController(getWindow(), getWindow().getDecorView());
        controller.setSystemBarsBehavior(
                WindowInsetsControllerCompat.BEHAVIOR_SHOW_TRANSIENT_BARS_BY_SWIPE);
        controller.hide(WindowInsetsCompat.Type.systemBars());
    }

    public void openUrl(String url) {
        runOnUiThread(() -> {
                    try {
                        Intent intent = new Intent(Intent.ACTION_VIEW, Uri.parse(url));
                        startActivity(intent);
                    } catch (ActivityNotFoundException e) {
                        Toast.makeText(this, "Can't open url", Toast.LENGTH_SHORT).show();
                    }
                }
        );
    }
}
