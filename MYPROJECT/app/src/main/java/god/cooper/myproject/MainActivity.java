package god.cooper.myproject;

import android.app.Notification;
import android.app.NotificationManager;
import android.content.Context;
import android.os.Bundle;
import android.support.v7.app.ActionBarActivity;
import android.webkit.JavascriptInterface;
import android.webkit.WebSettings;
import android.webkit.WebView;
import com.github.nkzawa.emitter.Emitter;
import com.github.nkzawa.socketio.client.IO;
import com.github.nkzawa.socketio.client.Socket;
import java.io.BufferedReader;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.URISyntaxException;
import java.net.URL;
import java.util.logging.Level;
import java.util.logging.Logger;


public class MainActivity extends ActionBarActivity {
    private DatabaseHelper mDatabaseHelper;

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

        // Enable JavaScript
        WebSettings webSettings = myWebView.getSettings();
        webSettings.setJavaScriptEnabled(true);
        myWebView.addJavascriptInterface(new MyJavaScriptInterface(this), "AndroidFunction");
        // Load HTML page
        myWebView.loadUrl("file:///android_asset/index.html");

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
    }
}
