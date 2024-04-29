import { useMemo, useState } from "react";
import { PermissionsAndroid, Platform } from "react-native";
import { BleManager, Device } from "react-native-ble-plx";
import * as ExpoDevice from "expo-device";

interface BluetoothLowEnergyApi {
  requestPermissions(): Promise<boolean>;
  scanForPeripherals(): void;
}

function useBLE(): BluetoothLowEnergyApi {
  const bleManager = useMemo(() => new BleManager(), []);

  const [allDevice, setAllDevices] = useState<Device[]>();

  const requestAndroidPermissions = async () => {
    const bluetoothScanPermission = await PermissionsAndroid.request(
      "android.permission.BLUETOOTH_SCAN",
      {
        title: "Scan Permission",
        message: "App requires Bluetooth Scanning",
        buttonPositive: "Ok",
      }
    );

    const bluetoothConnectPermission = await PermissionsAndroid.request(
      "android.permission.BLUETOOTH_CONNECT",
      {
        title: "Connection Permission",
        message: "App requires Bluetooth Connectivity",
        buttonPositive: "Ok",
      }
    );

    return (
      bluetoothScanPermission == "granted" &&
      bluetoothConnectPermission == "granted"
    );
  };

  const requestPermissions = async () => {
    if (Platform.OS === "android") {
      if ((ExpoDevice.platformApiLevel ?? -1) < 31) {
        // const granted = await PermissionsAndroid.request
        //
        return true;
      } else {
        const isAndroidPermissionGranted = await requestAndroidPermissions();
        return isAndroidPermissionGranted;
      }
    }

    return true;
  };

  const scanForPeripherals = () => {};

  return { requestPermissions, scanForPeripherals };
}

export default useBLE;
