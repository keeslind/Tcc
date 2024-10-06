"use client";
import mqtt, { MqttClient } from "mqtt";
import { Button } from "./ui/button";
import {
  Card,
  CardContent,
  CardDescription,
  CardFooter,
  CardHeader,
  CardTitle,
} from "./ui/card";

const client: MqttClient = mqtt.connect("mqtt://server.local:8085");
client.on("connect", () => {
  console.log("Connected");
});

function switchOn() {
  console.log("switch on");
  client.publish("LEO/led", "on");
}

function switchOff() {
  console.log("switch off");
  client.publish("LEO/led", "off");
}
const LedSwitch = () => {
  return (
    <Card>
      <CardHeader>
        <CardTitle>Licht</CardTitle>
        <CardDescription>
          Deze kaart laat zien hoe je vanuit MQTT op topic LEO/led de raspberry
          pico w led aan en uit kan zetten.
        </CardDescription>
      </CardHeader>
      <CardContent className="flex justify-around gap-8">
        <Button onClick={switchOn}>Led On</Button>
        <Button onClick={switchOff}>Led Off</Button>
      </CardContent>
      <CardFooter>
        <div className="flex justify-between min-w-full"></div>
      </CardFooter>
    </Card>
  );
};
export default LedSwitch;
