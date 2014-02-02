#pebble-emoncms 
A pebble watchapp for viewing current feed values and controlling setpoints.

# Building the app

The [Pebble developer guide](https://developer.getpebble.com/2/guides/) has installation and setup instructions for the Pebble SDK. 

Once you're up and running, just clone this project and run the following from the root of the project:

```
pebble build;
pebble install;
```

If the app isn't installed, you'll probably want to turn on developer mode on the Pebble app and set your PEBBLE_PHONE environment variable...

To get the debug messages just run

```
pebble logs
```

# To do

1. Custom units for each feed
2. Configuration screen to set emonCMS URL, feed list and API key
3. Bigger text for the feed values on the watch
4. Select button triggers "are you sure", or takes you to setpoint window for thermostat and posts to emonCMS

# License
This project is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

See <https://www.gnu.org/licenses/gpl-3.0-standalone.html>
