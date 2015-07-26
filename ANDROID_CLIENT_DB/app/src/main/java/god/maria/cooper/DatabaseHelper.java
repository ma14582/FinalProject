package god.maria.cooper;

import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;

public class DatabaseHelper extends SQLiteOpenHelper {
    public static final String DATABASE_NAME = "cooper.db";
    public static final String TABLE_NAME = "eventTable";
    public static final String COL_1 = "id";
    public static final String COL_2 = "serverId";
    public static final String COL_3 = "name";
    public static final String COL_4 = "event";
    public static final String COL_5 = "date";
    public static final String COL_6 = "time";

    public DatabaseHelper(Context context) {
        super(context, DATABASE_NAME, null, 1);
        SQLiteDatabase db = this.getWritableDatabase();
    }

    @Override
    public void onCreate(SQLiteDatabase db) {
        db.execSQL("CREATE TABLE " + TABLE_NAME + " (" + COL_1 + " INTEGER PRIMARY KEY AUTOINCREMENT, "
                + COL_2 + " INTEGER, " + COL_3 + " TEXT, " + COL_4 + " TEXT, " + COL_5 + " TEXT, "
                + COL_6 + " time TEXT)");
    }

    @Override
    public void onUpgrade(SQLiteDatabase db, int oldVersion, int newVersion) {
        db.execSQL("DROP TABLE IF EXISTS " + TABLE_NAME);
        onCreate(db);
    }

    public boolean insertData(int server_id, String name, String event, String date, String time){
        SQLiteDatabase db = this.getWritableDatabase();
        ContentValues contentValues = new ContentValues();
        contentValues.put(COL_2, server_id);
        contentValues.put(COL_3, name);
        contentValues.put(COL_4, event);
        contentValues.put(COL_5, date);
        contentValues.put(COL_6, time);
        long result = db.insert(TABLE_NAME, null, contentValues);
        if (result == -1){
            return false;
        } else {
            return true;
        }
    }
    public Cursor getAllData(){
        SQLiteDatabase db = this.getWritableDatabase();
        Cursor result = db.rawQuery("SELECT * FROM " + TABLE_NAME + " ORDER BY " + COL_2 + " DESC " + " LIMIT 10", null);
        return result;
    }

    public Cursor getServerIdAndroid(){
        SQLiteDatabase db = this.getWritableDatabase();
        Cursor result = db.rawQuery("SELECT serverId FROM " + TABLE_NAME, null);
        return result;
    }

}

