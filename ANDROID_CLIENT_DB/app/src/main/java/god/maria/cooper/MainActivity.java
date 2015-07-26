package god.maria.cooper;

import android.app.Notification;
import android.app.NotificationManager;
import android.content.Context;
import android.database.Cursor;
import android.os.Bundle;
import android.support.v7.app.ActionBarActivity;
import android.webkit.JavascriptInterface;
import android.webkit.WebSettings;
import android.webkit.WebView;

import com.github.nkzawa.emitter.Emitter;
import com.github.nkzawa.socketio.client.IO;
import com.github.nkzawa.socketio.client.Socket;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.BufferedReader;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.URISyntaxException;
import java.net.URL;
import java.util.HashSet;
import java.util.Set;
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
    Set<Integer> serverId = new HashSet<>();
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        myWebView = (WebView) this.findViewById(R.id.webView);
        myDb = new DatabaseHelper(this);
        //addEvent();
        serverId = serverIdBdAndroid();

        // Enable JavaScript
        WebSettings webSettings = myWebView.getSettings();
        webSettings.setJavaScriptEnabled(true);
        myWebView.addJavascriptInterface(new MyJavaScriptInterface(this), "AndroidFunction");
        // Load HTML page
        myWebView.loadUrl("file:///android_asset/db.html");

        try {
            socket = IO.socket("http://10.0.2.2:3003");
            //socket = IO.socket("http://192.168.0.3:3003");

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

    public Set<Integer> serverIdBdAndroid(){
        Set<Integer> result = new HashSet<>();
        Cursor resultQuery = myDb.getServerIdAndroid();
        while (resultQuery.moveToNext()) {
            int index = 0;
            result.add(resultQuery.getInt(index++));
        }
        resultQuery.close();
        return result;
    }

    public void addEvent() {
       // myDb.insertData(1, "Cooper", "bark", "01/01/2001", "10:00");
        //myDb.insertData(2, "Cooper", "mov", "02/02/2002", "20:00");
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
        public String getBarkFromServer(String service) throws JSONException {
            StringBuilder inputStringBuilder = new StringBuilder();
            String uri = "http://10.0.2.2:3000/"+ service;
            String result = null;
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

            result = inputStringBuilder.toString();

            JSONObject resultJson = new JSONObject(result);
            for(int i = 0; i < resultJson.getJSONArray("events").length(); i++) {
                JSONObject record = resultJson.getJSONArray("events").getJSONObject(i);
                if (!serverId.contains(record.getInt("id"))){
                    myDb.insertData(record.getInt("id"), record.getString("name"), record.getString("event"), record.getString("date"), record.getString("time"));
                }
            }
            return result;
        }

        @JavascriptInterface
        public String getBarkFromDb() throws JSONException {
            Cursor resultQuery = myDb.getAllData();
            String[] columnNames = resultQuery.getColumnNames();
            JSONArray jsonArray = new JSONArray();
            while(resultQuery.moveToNext()) {
                int index = 0;
                JSONObject record = new JSONObject();
                for(String columnName : columnNames) {
                    record.put(columnName, resultQuery.getString(index++));
                }
                jsonArray.put(record);
            }
            resultQuery.close();
            JSONObject jsonObject = new JSONObject();
            jsonObject.put("records", jsonArray);
            return jsonArray.toString();
        }
    }
}
