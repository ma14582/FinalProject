/*
 ==============================================================================================
 This class starts the app, creating the database, loading the webView and enabling the
 javascript interface.
 ==============================================================================================
*/

package god.maria.cooperdb;

import android.app.NotificationManager;
import android.content.Context;
import android.database.Cursor;
import android.os.Bundle;
import android.support.v7.app.ActionBarActivity;
import android.webkit.WebSettings;
import android.webkit.WebView;

import java.util.HashSet;
import java.util.Set;

public class MainActivity extends ActionBarActivity {

    // Creating objects of the different types
    private DatabaseAndroidActivity databaseAndroidActivity;
    private DatabaseHelper myDb;
    private ServerConnexion serverConnexion;
    private JavaScriptBridge javaScriptBridge;
    private WebView myWebView;

    // Starting the app, creating the database and enabling the webview and javascript interface
    @Override
    protected void onCreate(Bundle savedInstanceState) {

        this.myDb = new DatabaseHelper(this.getApplicationContext());
        this.serverConnexion = new ServerConnexion();
        this.databaseAndroidActivity = new DatabaseAndroidActivity(this.myDb);
        this.javaScriptBridge = new JavaScriptBridge(this.getApplicationContext(),
                (NotificationManager) (this.getSystemService(Context.NOTIFICATION_SERVICE)),
                this.myDb, this.serverConnexion, this.databaseAndroidActivity);

        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        myWebView = (WebView) this.findViewById(R.id.webView);

        // Enable JavaScript and Javascript interface
        WebSettings webSettings = myWebView.getSettings();
        webSettings.setJavaScriptEnabled(true);
        myWebView.addJavascriptInterface(this.javaScriptBridge, "AndroidFunction");
        // Load HTML page
        myWebView.loadUrl("file:///android_asset/index.html");
        this.javaScriptBridge.startTime();
    }
}
