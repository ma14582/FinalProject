package god.maria.cooperdb;

import android.database.Cursor;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

public class DatabaseAndroidActivity {
    private DatabaseHelper myDb;

    public DatabaseAndroidActivity(DatabaseHelper myDb) {
        this.myDb = myDb;
    }

    public String getEventFromDb(String service) throws JSONException {
        Cursor resultQuery = null;
        if (service.equals("getEvent")){
            resultQuery = myDb.eventQuery();        // 10 last events
        }
        if (service.equals("getBark")){
            resultQuery = myDb.barkQuery();         // 10 last barks
        }
        if (service.equals("getMovement")){
            resultQuery = myDb.movQuery();          // 10 last movements
        }
        String result = cursorToString(resultQuery);
        return result;
    }

    // Converting cursor result from rawQuery to String
    public String cursorToString(Cursor resultQuery) throws JSONException {
        String[] columnNames = resultQuery.getColumnNames();
        JSONArray jsonArray = queryValues(resultQuery, columnNames);
        JSONObject jsonObject = new JSONObject();
        jsonObject.put("events", jsonArray);
        return jsonObject.toString();
    }

    // Filling JsonArray with values from the rawQuery
    public JSONArray queryValues (Cursor resultQuery, String[] columnNames) throws JSONException{
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
        return jsonArray;
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
