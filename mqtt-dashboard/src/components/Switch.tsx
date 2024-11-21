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

function switchStraight() {
  client.publish("tcc/switch", "0,s"); // straight
}

function switchCurved() {
  client.publish("tcc/switch", "0,c"); // curved
}
function switchStraight1() {
  client.publish("tcc/switch", "1,s"); // straight
}

function switchCurved1() {
  client.publish("tcc/switch", "1,c"); // curved
}
const Switch = () => {
  return (
    <Card>
      <CardHeader>
        <CardTitle className="text-center">Licht</CardTitle>
        <hr className="border-2" />
        <CardDescription>How should i set the switch?</CardDescription>
      </CardHeader>
      <CardContent className="flex justify-around gap-8">
        <Button onClick={switchStraight}>0 - Straight</Button>
        <Button onClick={switchCurved}>0 - Curved</Button>
        <Button onClick={switchStraight1}>1 - Straight</Button>
        <Button onClick={switchCurved1}>1 - Curved</Button>
      </CardContent>
      <CardFooter>
        <div className="flex justify-between min-w-full"></div>
      </CardFooter>
    </Card>
  );
};
export default Switch;
