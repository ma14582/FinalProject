/*
 ==============================================================================================
 This class starts the app, creating the database, loading the webView and enabling the
 javascript interface. It also closes the websocket once the app is closed
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
    private DatabaseAndroidActivity databaseAndroidActivity;
    private DatabaseHelper myDb;
    private EventActivity eventActivity;
    private ServerConnexion serverConnexion;
    private WebSocketHelper webSocketHelper;
    private EventCache eventCache;
    private JavaScriptBridge javaScriptBridge;
    private WebView myWebView;

    // Starting the app, creating the database and enabling the webview and javascript interface
    @Override
    protected void onCreate(Bundle savedInstanceState) {

        this.myDb = new DatabaseHelper(this.getApplicationContext());
        this.eventActivity = new EventActivity(this.myDb);
        this.serverConnexion = new ServerConnexion();
        this.webSocketHelper = new WebSocketHelper();
        //this.eventCache = new EventCache(this.myDb);
        this.databaseAndroidActivity = new DatabaseAndroidActivity(this.myDb);
        this.javaScriptBridge = new JavaScriptBridge(this.getApplicationContext(),
                (NotificationManager) (this.getSystemService(Context.NOTIFICATION_SERVICE)),
                this.myDb, this.eventCache, this.serverConnexion, this.databaseAndroidActivity);

        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        myWebView = (WebView) this.findViewById(R.id.webView);

        //this.eventCache.init();

        // Enable JavaScript
        WebSettings webSettings = myWebView.getSettings();
        webSettings.setJavaScriptEnabled(true);
        myWebView.addJavascriptInterface(this.javaScriptBridge, "AndroidFunction");
        // Load HTML page
        myWebView.loadUrl("file:///android_asset/index.html");
        //Start Web Socket Connection
        this.webSocketHelper.connect();
    }

    // Closing the websocket once the app is closed
    @Override
    protected void onDestroy() {
        super.onDestroy();
        this.webSocketHelper.disconnect();
    }
}
