package main

import (
	"log"
	"os"

	mqtt "github.com/eclipse/paho.mqtt.golang"
)

func main() {
	mqtt.DEBUG = log.New(os.Stdout, "", 0)
	mqtt.ERROR = log.New(os.Stdout, "", 0)

	opts := mqtt.NewClientOptions().AddBroker("tcp://localhost:1883").SetClientID("radiance")
	c := mqtt.NewClient(opts)
	if token := c.Connect(); token.Wait() && token.Error() != nil {
		panic(token.Error())
	}

	token := c.Publish("testtopic", 0, false, "c")
	token.Wait()
}
