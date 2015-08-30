/*
 ==============================================================================================
 This class interfaces with the javascript, declaring all the methods which has been called
 from the web such as sending a notification for the user once a new event happens, getting
 all the events from the server if there is a connexion or , otherwise, from the database
 in android. It also inserts all the events in android database from the server every time user
 makes a request, updating them if there s connexion
 ==============================================================================================
*/

package god.maria.cooperdb;

import android.app.Notification;
import android.app.NotificationManager;
import android.content.Context;
import android.database.Cursor;
import android.webkit.JavascriptInterface;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;
import java.io.BufferedReader;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.URL;

public class JavaScriptBridge {

    private Context mContext;
    private ServerConnexion serverConnexion;
    private NotificationManager notificationManager;
    private Notification miNotification;
    private DatabaseHelper myDb;
    private EventCache eventCache;
    private static final int MY_NOTIFICATION_ID = 1;

    public JavaScriptBridge(Context c, NotificationManager notificationManager, DatabaseHelper myDb, EventCache eventCache, ServerConnexion serverConnexion) {
        this.mContext = c;
        this.notificationManager = notificationManager;
        this.myDb = myDb;
        this.eventCache = eventCache;
        this.serverConnexion = serverConnexion;
    }
    // Sending a notification for the user once a new event happens
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

        notificationManager.notify(MY_NOTIFICATION_ID, miNotification);
    }
    // getting all the events from the server if there is a connexion
    @JavascriptInterface
    public String getEventFromServer(String service) throws JSONException{
        String result = serverConnexion.connexionServer(service);
        insertDataInAndroidIfNew(result);
        return result;
    }

    // getting all the events from the android database if there is no connexion with the server
    @JavascriptInterface
    public String getBarkFromDb() throws JSONException {
        Cursor resultQuery = myDb.getAllData();
        String[] columnNames = resultQuery.getColumnNames();
        JSONArray jsonArray = new JSONArray();
        while (resultQuery.moveToNext()) {
            int index = 0;
            JSONObject record = new JSONObject();
            for (String columnName : columnNames) {
                record.put(columnName, resultQuery.getString(index++));
            }
            jsonArray.put(record);
        }
        resultQuery.close();
        JSONObject jsonObject = new JSONObject();
        jsonObject.put("records", jsonArray);
        return jsonArray.toString();
    }

    // inserting all the events in android database from the server
    public void insertDataInAndroidIfNew(String result) throws JSONException{
        JSONObject resultJson = new JSONObject(result);
        for (int i = 0; i < resultJson.getJSONArray("events").length(); i++) {
            JSONObject record = resultJson.getJSONArray("events").getJSONObject(i);
            this.myDb.insertData(record.getInt("id"), record.getString("name"), record.getString("event"), record.getString("date"), record.getString("time"));
        }
    }
}