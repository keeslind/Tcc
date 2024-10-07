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
  client.subscribe("LEO/temp");
  console.log("Subscibed");
});

const Temperature = () => {
  const [temperature, setTemperature] = useState("");
  useEffect(() => {
    client.on("message", (topic, payload) => {
      console.log(`Received message ${count++}`);
      setTemperature(payload.toString());
      return () => {
        console.log("Unsubscribing");
        client.unsubscribe("LEO/temp");
      };
    });
  }, []);

  return (
    <Card>
      <CardHeader>
        <CardTitle className="text-center">Temperatuur</CardTitle>
        <hr className="border-2" />
        <CardDescription>
          Deze kaart laat temperatuur zien die vanuit MQTT op topic LEO/temp
          binnenkomt.
        </CardDescription>
      </CardHeader>
      <CardContent className="text-center">
        <p>{temperature}</p>
      </CardContent>
      <CardFooter>
        <div className="flex justify-between min-w-full"></div>
      </CardFooter>
    </Card>
  );
};

export default Temperature;
