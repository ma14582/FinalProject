/*
 ==============================================================================================
 This class declares all the variables for the database and table, creates the tableEvent and
 implements the method to insert data in the database
 ==============================================================================================
*/

package god.maria.cooperdb;

import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;

public class DatabaseHelper extends SQLiteOpenHelper {

    public static final String DATABASE_NAME = "cooper.db";
    public static final String TABLE_NAME = "eventTable";
    public static final String COL_ID = "id";
    public static final String COL_NAME = "name";
    public static final String COL_EVENT = "event";
    public static final String COL_DATE = "date";
    public static final String COL_TIME = "time";

    public DatabaseHelper(Context context) {
        super(context, DATABASE_NAME, null, 1);
    }

    // Creating Table
    @Override
    public void onCreate(SQLiteDatabase db) {
        db.execSQL("CREATE TABLE " + TABLE_NAME + " (" + COL_ID + " INTEGER PRIMARY KEY, "
                + COL_NAME + " TEXT, " + COL_EVENT + " TEXT, " + COL_DATE + " TEXT, "
                + COL_TIME + " time TEXT)");
    }

    @Override
    public void onUpgrade(SQLiteDatabase db, int oldVersion, int newVersion) {
        db.execSQL("DROP TABLE IF EXISTS " + TABLE_NAME);
        onCreate(db);
    }

    // Getting the last 10 events included in the android database according to a rawquery
    public Cursor getAllData(){
        Cursor result = this.getReadableDatabase().rawQuery("SELECT * FROM " + TABLE_NAME + " ORDER BY " + COL_ID + " DESC " + " LIMIT 10", null);
        return result;
    }

    // Inserting data
    public void insertData(int server_id, String name, String event, String date, String time) {
        ContentValues contentValues = new ContentValues();
        contentValues.put(COL_ID, server_id);
        contentValues.put(COL_NAME, name);
        contentValues.put(COL_EVENT, event);
        contentValues.put(COL_DATE, date);
        contentValues.put(COL_TIME, time);
        this.getWritableDatabase().insert(TABLE_NAME, null, contentValues);
    }
}

