package com.example.ballplateremote

import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothDevice
import android.bluetooth.BluetoothSocket
import android.content.Intent
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.os.Handler
import android.util.Log
import android.widget.Button
import android.widget.Toast
import android.widget.ToggleButton
import java.io.IOException
import java.io.InputStream
import java.io.OutputStream
import java.util.*

class MainActivity : AppCompatActivity() {
    private val TAG = "MainActivity"

    private var socket: BluetoothSocket? = null
    private var outputStream: OutputStream? = null
    private var inputStream: InputStream? = null

    private val handler = Handler()
    private var runnable: Runnable? = null
    private val delay = 100

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
    }
    override fun onResume() {
        super.onResume()

        startBluetooth()

        handler.postDelayed(Runnable {
            handler.postDelayed(runnable, delay.toLong())
            sendCommand()
        }.also { runnable = it }, delay.toLong())
    }

    override fun onPause() {
        super.onPause()
        handler.removeCallbacks(runnable)
    }

    private fun sendCommand() {
        super.onResume()

        val centreButton: Button = findViewById(R.id.centreButton)
        val squareButton: Button = findViewById(R.id.squareButton)
        val circleButton: Button = findViewById(R.id.circleButton)

        when {
            centreButton.isPressed -> {
                Log.v(TAG, "centreButton")
                outputStream?.write('1'.toInt());
            }
            squareButton.isPressed -> {
                Log.v(TAG, "squareButton")
                outputStream?.write('2'.toInt());
            }
            circleButton.isPressed -> {
                Log.v(TAG, "circleButton")
                outputStream?.write('3'.toInt());
            }
        }
    }

    private fun toast(text: String) {
        Toast.makeText(this, text, Toast.LENGTH_SHORT).show()
    }

    private fun startBluetooth() {
        val bluetoothAdapter = BluetoothAdapter.getDefaultAdapter()
        if (bluetoothAdapter == null) {
            toast("No bluetooth adapter available")
            return
        }
        if (!bluetoothAdapter.isEnabled) {
            val enableBluetooth = Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE)
            startActivityForResult(enableBluetooth, 0)
        }
        val pairedDevices: Set<BluetoothDevice> = bluetoothAdapter.bondedDevices
        val device = pairedDevices.find { d -> d.name == "BallPlate" }
        if (device == null) {
            toast("BallPlate NOT Found")
        } else {
            // Standard SerialPortService ID
            val uuid: UUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB")
            val socket1 = device.createRfcommSocketToServiceRecord(uuid)
            try {
                socket1.connect()
                outputStream = socket1.getOutputStream()
                inputStream = socket1.getInputStream()
                socket = socket1
                toast("Bluetooth Opened")
            } catch (e: IOException) {
                toast("Exception: $e")
            }
        }
    }
}