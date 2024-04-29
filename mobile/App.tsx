// import { StatusBar } from "expo-status-bar";
import { StyleSheet, Text, View } from "react-native";
import Connect from "./components/Connect";
import useBLE from "./hooks/useBLE";

export default function App() {
  const { requestPermissions, scanForPeripherals } = useBLE();

  const connectToDevice = async () => {
    const isPermissionEnabled = await requestPermissions();
    if (isPermissionEnabled) {
      scanForPeripherals();
    }
  };

  return (
    <View style={styles.container}>
      <Text style={styles.heading}>Carbon Device Tracker </Text>
      <Connect onConnectRequest={connectToDevice} />
      {/* <StatusBar style="auto" /> */}
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    backgroundColor: "#082f49",
    alignItems: "center",
    justifyContent: "flex-start",
  },

  heading: {
    color: "#ddd",
    fontSize: 26,
    width: "100%",
    paddingTop: 40,
    textAlign: "center",
    backgroundColor: "#164e63",
    paddingVertical: 10,
  },
});
