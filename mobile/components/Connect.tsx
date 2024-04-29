import { StyleSheet, Text, TouchableOpacity, View } from "react-native";

interface Props {
  onConnectRequest: () => void;
}

export default function Connect({ onConnectRequest }: Props) {
  return (
    <View style={styles.container}>
      <Text style={styles.deviceText}>Bluetooth</Text>
      <TouchableOpacity onPress={onConnectRequest} style={styles.button}>
        <Text style={styles.buttonText}>Connect</Text>
      </TouchableOpacity>
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    width: "70%",
    display: "flex",
    flexDirection: "row",
    alignItems: "center",
    justifyContent: "space-evenly",
    padding: 10,
    borderRadius: 3,
    borderColor: "#075985",
    borderWidth: 3,
    marginTop: 10,
  },
  deviceText: {
    color: "#aaa",
    paddingHorizontal: 10,
  },

  button: {
    backgroundColor: "#075985",
    paddingHorizontal: 20,
    paddingVertical: 10,
  },
  buttonText: {
    color: "#aaa",
  },
});
