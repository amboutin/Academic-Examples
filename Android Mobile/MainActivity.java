package com.example.user.sqlite_loc;

import android.Manifest;
import android.app.Dialog;
import android.content.ContentValues;
import android.content.Context;
import android.content.pm.PackageManager;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;
import android.location.Location;
import android.provider.BaseColumns;
import android.support.annotation.NonNull;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.EditText;
import android.widget.ListView;
import android.widget.SimpleCursorAdapter;
import com.google.android.gms.common.ConnectionResult;
import com.google.android.gms.common.GoogleApiAvailability;
import com.google.android.gms.common.api.GoogleApiClient;
import com.google.android.gms.location.LocationListener;
import com.google.android.gms.location.LocationRequest;
import com.google.android.gms.location.LocationServices;

public class MainActivity extends AppCompatActivity implements
        GoogleApiClient.ConnectionCallbacks, GoogleApiClient.OnConnectionFailedListener {

    private static final String TAG = "6";
    private SimpleCursorAdapter mSQLCursorAdapter;
    private Cursor mSQLCursor;
    private SQLiteExample mSQLiteExample;
    private SQLiteDatabase mSQLDB;
    private GoogleApiClient mGoogleApiClient;
    private LocationRequest mLocationRequest;
    private LocationListener mLocationListener;
    private static final int LOCATION_PERMISSION_RESULT = 17;
    private static final int MY_PERMISSIONS_REQUEST_ACCESS_FINE_LOCATION = 18;
    public Location mLastLocation;
    public double LAT = 44.5;
    public double LONG = -123.2;
    public int once = 0;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        mLocationRequest = LocationRequest.create();
        mLocationRequest.setPriority(LocationRequest.PRIORITY_HIGH_ACCURACY);
        mLocationRequest.setInterval(5000);
        mLocationRequest.setFastestInterval(5000);
        mLocationListener = new LocationListener() {

            @Override
            public void onLocationChanged(Location location) {
                if (location != null) {
                    LAT = location.getLatitude();
                    LONG = location.getLongitude();
                } else {
                    LAT = 44.5;
                    LONG = -123.2;
                }
            }
        };
         if (mGoogleApiClient == null) {
            mGoogleApiClient = new GoogleApiClient.Builder(this)
                    .addConnectionCallbacks(this)
                    .addOnConnectionFailedListener(this)
                    .addApi(LocationServices.API)
                    .build();
        }
        mGoogleApiClient.connect();
        mSQLiteExample = new SQLiteExample(this);
        mSQLDB = mSQLiteExample.getWritableDatabase();
     populateTable();
    }




    /** Called when the user taps the Send button */
    public void submitText(View view) {
        // Do something in response to button
        if (ContextCompat.checkSelfPermission(this, Manifest.permission.ACCESS_FINE_LOCATION) != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.ACCESS_FINE_LOCATION}, MY_PERMISSIONS_REQUEST_ACCESS_FINE_LOCATION);
        } else {
            // Insert DB and Populate Table
            updateLocation();
            if (mSQLDB != null) {
                ContentValues testValues = new ContentValues();
                testValues.put(DBContract.LocationTable.COLUMN_NAME_NOTE, ((EditText) findViewById(R.id.editView)).getText().toString());
                testValues.put(DBContract.LocationTable.COLUMN_NAME_DEMO_LAT, String.valueOf(LAT));
                testValues.put(DBContract.LocationTable.COLUMN_NAME_DEMO_LONG, String.valueOf(LONG));
                mSQLDB.insert(DBContract.LocationTable.TABLE_NAME, null, testValues);
                populateTable();
            } else {
                Log.d(TAG, "Unable to access database for writing.");
            }
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults){
        if(requestCode == MY_PERMISSIONS_REQUEST_ACCESS_FINE_LOCATION){
            if(grantResults.length > 0){
                updateLocation();
                if (mSQLDB != null) {
                    ContentValues testValues = new ContentValues();
                    testValues.put(DBContract.LocationTable.COLUMN_NAME_NOTE, ((EditText) findViewById(R.id.editView)).getText().toString());
                    testValues.put(DBContract.LocationTable.COLUMN_NAME_DEMO_LAT, String.valueOf(LAT));
                    testValues.put(DBContract.LocationTable.COLUMN_NAME_DEMO_LONG, String.valueOf(LONG));
                    mSQLDB.insert(DBContract.LocationTable.TABLE_NAME, null, testValues);
                    populateTable();
                } else {
                    Log.d(TAG, "Unable to access database for writing.");
                }
            }
            else{
                updateLocation();
                if (mSQLDB != null) {
                    ContentValues testValues = new ContentValues();
                    testValues.put(DBContract.LocationTable.COLUMN_NAME_NOTE, ((EditText) findViewById(R.id.editView)).getText().toString());
                    testValues.put(DBContract.LocationTable.COLUMN_NAME_DEMO_LAT, String.valueOf(LAT));
                    testValues.put(DBContract.LocationTable.COLUMN_NAME_DEMO_LONG, String.valueOf(LONG));
                    mSQLDB.insert(DBContract.LocationTable.TABLE_NAME, null, testValues);
                    populateTable();
                } else {
                    Log.d(TAG, "Unable to access database for writing.");
                }

            }
        }
    }

    private void updateLocation() {
        if (ActivityCompat.checkSelfPermission(this, android.Manifest.permission.ACCESS_FINE_LOCATION) != PackageManager.PERMISSION_GRANTED) {
            LAT = 44.5;
            LONG = -123.2;
        } else {
            //   }
            if(once == 0){
            mLastLocation = LocationServices.FusedLocationApi.getLastLocation(mGoogleApiClient);

            if (mLastLocation != null) {
                LONG = mLastLocation.getLongitude();
                LAT = mLastLocation.getLatitude();
                }
                once = 1;
            }
                LocationServices.FusedLocationApi.requestLocationUpdates(mGoogleApiClient, mLocationRequest, mLocationListener);
            }
        }

    @Override
    public void onConnectionFailed(@NonNull ConnectionResult connectionResult){
        Dialog errDialog = GoogleApiAvailability.getInstance().getErrorDialog(this, connectionResult.getErrorCode(),0);
        errDialog.show();
    }

    @Override
    public void onConnectionSuspended(int value){
        Dialog errDialog = GoogleApiAvailability.getInstance().getErrorDialog(this, CAUSE_NETWORK_LOST,0);
        errDialog.show();
    }

    @Override
    public void onConnected(Bundle bundle) {
        if (ActivityCompat.checkSelfPermission(this, android.Manifest.permission.ACCESS_FINE_LOCATION) != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.ACCESS_FINE_LOCATION}, LOCATION_PERMISSION_RESULT);
        }
        updateLocation();
    }

    public void populateTable(){
        if(mSQLDB != null){
            try{
                if(mSQLCursorAdapter != null && mSQLCursorAdapter.getCursor() != null){
                    if(!mSQLCursorAdapter.getCursor().isClosed()){
                        mSQLCursorAdapter.getCursor().close();
                    }
                }
                mSQLCursor = mSQLDB.query(DBContract.LocationTable.TABLE_NAME,
                        new String[]{DBContract.LocationTable._ID, DBContract.LocationTable.COLUMN_NAME_NOTE,
                                DBContract.LocationTable.COLUMN_NAME_DEMO_LAT, DBContract.LocationTable.COLUMN_NAME_DEMO_LONG }, null, null, null, null, null);
                ListView SQLListView = (ListView) findViewById(R.id.SQLView);
                mSQLCursorAdapter = new SimpleCursorAdapter(this,
                        R.layout.sql_list_view, // was layout
                        mSQLCursor,
                        new String[]{DBContract.LocationTable.COLUMN_NAME_NOTE, DBContract.LocationTable.COLUMN_NAME_DEMO_LAT, DBContract.LocationTable.COLUMN_NAME_DEMO_LONG},
                        new int[]{R.id.textViewString, R.id.textViewLat, R.id.textViewLong},
                        0);
                SQLListView.setAdapter(mSQLCursorAdapter);
            } catch (Exception e) {
                Log.d(TAG, "Error loading data from database");
            }
        }

    }
}

final class DBContract {
    private DBContract(){}

    public final class LocationTable implements BaseColumns {
        public static final String DB_NAME = "locDB";
        public static final String TABLE_NAME = "savedLoc";
        public static final String COLUMN_NAME_NOTE = "String";
        public static final String COLUMN_NAME_DEMO_LAT = "LAT";
        public static final String COLUMN_NAME_DEMO_LONG = "LONG";
        public static final int DB_VERSION = 6;


        public static final String SQL_CREATE_SAVEDLOC_TABLE = "CREATE TABLE " +
                LocationTable.TABLE_NAME + "(" + LocationTable._ID + " INTEGER PRIMARY KEY NOT NULL," +
                LocationTable.COLUMN_NAME_NOTE + " VARCHAR(255)," +
                LocationTable.COLUMN_NAME_DEMO_LAT + " VARCHAR(255)," +
                LocationTable.COLUMN_NAME_DEMO_LONG + " VARCHAR(255));";

        public  static final String SQL_DROP_DEMO_TABLE = "DROP TABLE IF EXISTS " + LocationTable.TABLE_NAME;
    }
}

class SQLiteExample extends SQLiteOpenHelper {

    public SQLiteExample(Context context) {
        super(context, DBContract.LocationTable.DB_NAME, null, DBContract.LocationTable.DB_VERSION);
    }

    @Override
    public void onCreate(SQLiteDatabase db) {
        db.execSQL(DBContract.LocationTable.SQL_CREATE_SAVEDLOC_TABLE);
    }

    @Override
    public void onUpgrade(SQLiteDatabase db, int oldVersion, int newVersion) {
        db.execSQL(DBContract.LocationTable.SQL_DROP_DEMO_TABLE);
        onCreate(db);
    }
}