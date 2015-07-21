package god.cooper.myproject;

import android.app.AlertDialog;
import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.net.Uri;
import android.support.v7.app.ActionBarActivity;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.webkit.JavascriptInterface;
import android.webkit.WebSettings;
import android.webkit.WebView;
import android.widget.Toast;


public class MainActivity extends ActionBarActivity {
    private final String miBlog = "http://probando-cosas.com.ar/";
    NotificationManager notificationManager;
    Notification miNotification;
    private static final int MY_NOTIFICATION_ID = 1;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        WebView myWebView = (WebView) this.findViewById(R.id.webView);

        // Enable JavaScript
        WebSettings webSettings = myWebView.getSettings();
        webSettings.setJavaScriptEnabled(true);
        myWebView.addJavascriptInterface(new MyJavaScriptInterface(this), "AndroidFunction");
        // Load HTML page
        myWebView.loadUrl("file:///android_asset/insertBark.html");
    }

    public class MyJavaScriptInterface {
        Context mContext = getApplicationContext();
        Intent miIntent = new Intent(Intent.ACTION_VIEW, Uri.parse(miBlog));
        MyJavaScriptInterface(Context c) {
            mContext = c;
        }

        @JavascriptInterface
        public void openAndroidDialog(){
            PendingIntent pendingIntent = PendingIntent.getActivity( MainActivity.this, 0, miIntent, Intent.FLAG_ACTIVITY_NEW_TASK);

            miNotification = new Notification.Builder(mContext)
                    .setContentTitle("Es hora de visitar ...")
                    .setContentText("http://probando-cosas.com.ar/")
                    .setTicker("Notificación!")
                    .setWhen(System.currentTimeMillis())
                    .setContentIntent(pendingIntent)
                    .setDefaults(Notification.DEFAULT_SOUND)
                    .setAutoCancel(true)
                    .setSmallIcon(R.drawable.ic_launcher)
                    .build();
            notificationManager = (NotificationManager)getSystemService(Context.NOTIFICATION_SERVICE);
            notificationManager.notify(MY_NOTIFICATION_ID, miNotification);
        }

    }
}
