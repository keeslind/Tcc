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

function turnoutStraight() {
  client.publish("tcc/turnout", "0,s"); // straight
}

function turnoutCurved() {
  client.publish("tcc/turnout", "0,c"); // curved
}
function turnoutStraight1() {
  client.publish("tcc/turnout", "1,s"); // straight
}

function turnoutCurved1() {
  client.publish("tcc/turnout", "1,c"); // curved
}
const Turnout = () => {
  return (
    <Card>
      <CardHeader>
        <CardTitle className="text-center">Turnouts</CardTitle>
        <hr className="border-2" />
        <CardDescription>How should i set the turnout?</CardDescription>
      </CardHeader>
      <CardContent className="flex justify-around gap-8">
        <Button onClick={turnoutStraight}>0 - Straight</Button>
        <Button onClick={turnoutCurved}>0 - Curved</Button>
        <Button onClick={turnoutStraight1}>1 - Straight</Button>
        <Button onClick={turnoutCurved1}>1 - Curved</Button>
      </CardContent>
      <CardFooter>
        <div className="flex justify-between min-w-full"></div>
      </CardFooter>
    </Card>
  );
};
export default Turnout;
