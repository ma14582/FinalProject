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

public class JavaScriptBridge {

    // Creating objects of the different types
    private DatabaseAndroidActivity databaseAndroidActivity;
    private Context mContext;
    private ServerConnexion serverConnexion;
    private NotificationManager notificationManager;
    private Notification miNotification;
    private DatabaseHelper myDb;
    private static final int MY_NOTIFICATION_ID = 1;

    // JavaScriptBridge Constructor
    public JavaScriptBridge(Context c, NotificationManager notificationManager, DatabaseHelper myDb, ServerConnexion serverConnexion, DatabaseAndroidActivity databaseAndroidActivity) {
        this.mContext = c;
        this.notificationManager = notificationManager;
        this.myDb = myDb;
        //this.eventCache = eventCache;
        this.serverConnexion = serverConnexion;
        this.databaseAndroidActivity = databaseAndroidActivity;
    }
    // Sending a notification to the user once a new event happens
    @JavascriptInterface
    public void sendNotificationUser(String message, String newEvent) throws JSONException{
        miNotification = new Notification.Builder(mContext)
                .setContentTitle("It is time to check  ...")
                .setContentText(message)
                .setTicker("Notificatión!")
                .setWhen(System.currentTimeMillis())
                .setDefaults(Notification.DEFAULT_SOUND)
                .setAutoCancel(true)
                .setSmallIcon(R.drawable.ic_launcher)
                .build();

        notificationManager.notify(MY_NOTIFICATION_ID, miNotification);
        databaseAndroidActivity.insertDataInAndroidIfNew(newEvent);
    }
    // getting all the events from the server or the mobile database, according to the server connexion
    @JavascriptInterface
    public String getEventFromServerOrAndroidDb(String service) throws JSONException{
        String result = null;
        if (serverConnexion.resultFromServer(service).equals("no server connexion")) {
            result = databaseAndroidActivity.getEventFromDb(service);
        } else {
            result = serverConnexion.resultFromServer(service);
            databaseAndroidActivity.insertDataInAndroidIfNew(result);
        }
        return result;
    }
}