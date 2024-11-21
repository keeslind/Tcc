"use client";

import {
  Card,
  CardContent,
  CardDescription,
  CardFooter,
  CardHeader,
  CardTitle,
} from "./ui/card";
import { useEffect, useState } from "react";
import mqtt, { MqttClient } from "mqtt";

let count = 1;

const client: MqttClient = mqtt.connect("mqtt://server.local:8085");
client.on("connect", () => {
  console.log("Connected");
  client.subscribe("tcc/train");
  console.log("Subscribed");
});

const Train = () => {
  const [train, setTrain] = useState("");
  useEffect(() => {
    client.on("message", (topic, payload) => {
      console.log(`Received message ${count++}`);
      setTrain(payload.toString());
      return () => {
        console.log("Unsubscribing");
        client.unsubscribe("tcc/train");
      };
    });
  }, []);

  return (
    <Card>
      <CardHeader>
        <CardTitle className="text-center">Train</CardTitle>
        <hr className="border-2" />
        <CardDescription>
          Deze kaart laat de NFC gedetecteerde trein zien.
        </CardDescription>
      </CardHeader>
      <CardContent className="text-center">
        <p>{train}</p>
      </CardContent>
      <CardFooter>
        <div className="flex justify-between min-w-full"></div>
      </CardFooter>
    </Card>
  );
};

export default Train;
