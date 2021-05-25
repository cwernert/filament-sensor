# filament-sensor
Detects blockage, exhaustion &amp; completion of my 3D printer

## Dependancies
You will need into install [Arduino core for ESP8266 WiFi chip](https://github.com/esp8266/Arduino) in order to use this sketch.
The sketch also uses a *deprecated* HTTP Client function (ESP8266HTTPClient.h) so you'll need to select Version 2.7.4 when installing the ESP8266 IDE via Arduino Boards Manager.
(The ESP8266 Community released an update literally a day after I finished the sketch, so there's probably a good reason why this HTTP client is now deprecated but I'm not worried.)

In its current form, the sketch also relies on [Zapier](https://zapier.com) as the endpoint for the alerts (because I was too lazy to read Discord's API documentation). However, this does mean it would be very easy to customise to that you can be notifed on the app of your choice - for example, if you'd prefer notifications to go to Slack.

As soon as you create a Zap in Zapier which uses the [Webhooks By Zapier trigger, "Catch Hook"](https://zapier.com/apps/webhook/integrations#triggers-and-actions) - a custom Webhook URL will be generated which you can copy/paste into the sketch.

## Setup
This is written for a [WEMOS D1 Mini](https://www.wemos.cc/en/latest/d1/d1_mini.html), along with some additional components which can be found [on Reddit here](https://www.reddit.com/r/functionalprint/comments/nkl51o/i_made_a_device_which_lets_me_know_when_my_print/). I'll post the 3D models on Thingiverse at some point.

There are a few constant variables to declare at the start, such as your WiFi credentials and the Zapier Webhook URL. You can also adjust the alert messages if you'd like to.

Depending on your printer, you might also like to play with the detection threshold (set to 4000ms for my printer) to improve detection speed without introducing false-positives.
