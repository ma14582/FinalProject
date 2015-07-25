package god.cooper.myproject;

import android.app.Notification;
import android.app.NotificationManager;
import android.content.Context;
import android.database.Cursor;
import android.os.Bundle;
import android.support.v7.app.ActionBarActivity;
import android.util.Log;
import android.webkit.JavascriptInterface;
import android.webkit.WebSettings;
import android.webkit.WebView;
import com.github.nkzawa.emitter.Emitter;
import com.github.nkzawa.socketio.client.IO;
import com.github.nkzawa.socketio.client.Socket;

import org.json.JSONArray;
import org.json.JSONObject;

import java.io.BufferedReader;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.URISyntaxException;
import java.net.URL;
import java.util.ArrayList;
import java.util.logging.Level;
import java.util.logging.Logger;

public class MainActivity extends ActionBarActivity {
    DatabaseHelper myDb;

    //private final String miBlog = "http://probando-cosas.com.ar/";
    NotificationManager notificationManager;
    Notification miNotification;
    private static final int MY_NOTIFICATION_ID = 1;
    WebView myWebView;
    Socket socket;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        myWebView = (WebView) this.findViewById(R.id.webView);
        myDb = new DatabaseHelper(this);
        //addEvent();

        // Enable JavaScript
        WebSettings webSettings = myWebView.getSettings();
        webSettings.setJavaScriptEnabled(true);
        myWebView.addJavascriptInterface(new MyJavaScriptInterface(this), "AndroidFunction");
        // Load HTML page
        myWebView.loadUrl("file:///android_asset/db.html");

        try {
            socket = IO.socket("http://10.0.2.2:3003");
            socket
                    .on(Socket.EVENT_CONNECT_ERROR, new Emitter.Listener() {
                        @Override
                        public void call(Object... args) {
                            System.out.println("connect error");
                        }
                    })
                    .on(Socket.EVENT_CONNECT, new Emitter.Listener() {

                        @Override
                        public void call(Object... args) {
                            socket.emit("update", "hi");
                        }

                    })
                    .on("event", new Emitter.Listener() {

                        @Override
                        public void call(Object... args) {
                        }
                    })
                    .on(Socket.EVENT_DISCONNECT, new Emitter.Listener() {

                        @Override
                        public void call(Object... args) {
                            System.out.println("disconnect");
                        }

                    });
            socket.connect();
            while (!socket.connected()) {

            }
            socket.emit("update", "Hello World!");

            socket.disconnect();
        } catch (URISyntaxException ex) {
            Logger.getLogger(MainActivity.class.getName()).log(Level.SEVERE, null, ex);
        }
    }

    public void addEvent() {
        myDb.insertData("Cooper", "bark", "01/01/2001", "10:00");
        myDb.insertData("Cooper", "mov", "02/02/2002", "20:00");
    }

    public class MyJavaScriptInterface {
        Context mContext = getApplicationContext();

        //Intent miIntent = new Intent(Intent.ACTION_VIEW, Uri.parse(miBlog));
        MyJavaScriptInterface(Context c) {
            mContext = c;
        }

        @JavascriptInterface
        public void openAndroidDialog() {
            //PendingIntent pendingIntent = PendingIntent.getActivity( MainActivity.this, 0, miIntent, Intent.FLAG_ACTIVITY_NEW_TASK);
            miNotification = new Notification.Builder(mContext)
                    .setContentTitle("It is time to check  ...")
                    .setContentText("Cooper")
                    .setTicker("Notificatión!")
                    .setWhen(System.currentTimeMillis())
                            //.setContentIntent(pendingIntent)
                    .setDefaults(Notification.DEFAULT_SOUND)
                    .setAutoCancel(true)
                    .setSmallIcon(R.drawable.ic_launcher)
                    .build();
            notificationManager = (NotificationManager) getSystemService(Context.NOTIFICATION_SERVICE);
            notificationManager.notify(MY_NOTIFICATION_ID, miNotification);
        }

        @JavascriptInterface
        public String getBarkFromServer(String service) {
            StringBuilder inputStringBuilder = new StringBuilder();
            String uri = "http://10.0.2.2:3000/"+ service;
            URL url = null;
            try {
                url = new URL(uri);
                HttpURLConnection connection = (HttpURLConnection) url.openConnection();
                connection.setRequestMethod("GET");
                connection.setRequestProperty("Accept", "application/json");
                InputStream data = connection.getInputStream();
                connection.disconnect();
                BufferedReader bufferedReader = new BufferedReader(new InputStreamReader(data, "UTF-8"));
                String line = bufferedReader.readLine();
                while(line != null){
                    inputStringBuilder.append(line);inputStringBuilder.append('\n');
                    line = bufferedReader.readLine();
                }
            } catch (Exception e) {
                e.printStackTrace();
            }
            return inputStringBuilder.toString();
        }

        @JavascriptInterface
        public String getBarkFromDb() {
            String a = null, b = null, c = null, d = null, e = null;
            Cursor resultQuery = myDb.getAllData();
            while (resultQuery.moveToNext()) {
                a = resultQuery.getString(0);
                b = resultQuery.getString(1);
                c = resultQuery.getString(2);
                d = resultQuery.getString(3);
                e = resultQuery.getString(4);
            }
            String abcde = a + " " + b + " " + c + " " + d + " " + e;
            resultQuery.close();
            return abcde;
        }
    }
}
