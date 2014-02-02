var emon;
var feedList = ['26349', '26351'];

Pebble.addEventListener("ready", function(){
    Pebble.sendAppMessage({
      "feedCount": feedList.length
    }); 
    // Replace null with your API key (TODO add this to config screen)
    emon = emonFactory("http://emoncms.org", feedList, null, 60);
  }
);

Pebble.addEventListener("appmessage",
  function(e) {
    console.log("Received message:");
    console.log(" state: " + e.payload.state);
    console.log(" index: " + e.payload.feedIndex);
    if(e.payload.feedIndex != undefined && e.payload.feedIndex != null){
      feedIndex = e.payload.feedIndex;
      console.log("updating pebble: "+e.payload.feedIndex);
      if(emon){
        console.log("...telling emon")
        emon.setFeedIndex(e.payload.feedIndex);
      }else{
        console.log("App not ready yet.");
      }
    }
  }
);


/**
 The emoncms interface. This object does the back-and-forth with the emoncms instance
**/
var emonFactory = function(emonServer, feeds, apiKey, refreshRateSecs){

  var feedIndex = 0;
  var _emonServer = emonServer;
  var _apiKey = apiKey;
  var _feedList = {};


  function getFeedValue(feedId, onComplete){
      var url = _emonServer+'/feed/value.json?id='+feedId;
      if(_apiKey){
        url = url+'&apikey='+_apiKey;
      }
      var feedValue = null; // the response object
      var req = new XMLHttpRequest();
      req.open('GET', url, true);
      req.onload = function(e) {
        if (req.readyState == 4 && req.status == 200) {
          if (req.status == 200) {
            feedValue = req.responseText;
            feedValue = feedValue.substring(1, feedValue.length - 1);
            console.log(":" + feedValue)
            onComplete(feedId, feedValue);
          } else {
            console.log("Something bad happened getting the updated value. Maybe no internet connection...");
          }
        }
      }
      req.send(null);
  }

  function updatePebbleIfChanged(feedId, newValue){

    var isUpdated = newValue !== _feedList[feedId].value;

    // Update the timestamp and value;
    _feedList[feedId].lastChecked = new Date();
    _feedList[feedId].value = newValue ;

    if(feedId == feedList[feedIndex] && isUpdated){
      console.log("feed "+feedId+" changed to "+newValue);
      updatePebble(_feedList[feedId]);
      console.log("updated");
    }
  }

  function updatePebble(feed){
    console.log("sending "+feed.value+" for "+feed.feedId);
    Pebble.sendAppMessage({
      "temperature": feed.value + feed.units
    }); 
  }

  /**
   *  Initialise everything
   **/
  for (var i = 0; i < feeds.length; i++) {
    _feedList[feeds[i]] = {
      feedId: feeds[i],
      units: "\u00B0C",
      lastChecked: new Date()
    }
    getFeedValue(feeds[i], updatePebbleIfChanged)
  };

  /**
   * Update the feed values every minute
  **/
  setInterval(function() {

    for(var feedId in _feedList){
      getFeedValue(feedId, updatePebbleIfChanged);
    }
  }, refreshRateSecs *1000);// Multiple seconds up to millis

  return {
    setFeedIndex: function(newIndex){
      feedIndex = newIndex;
      updatePebble(_feedList[feedList[feedIndex]]);
    },
    currentValues: function(){
      return _feedList;
    },
    getValue: function(feedId){
      return _feedList[feedId].value;
    },
    updateValue: function(feedId, value){
      this.setPoints[feedId] = value;
    }
  }
};


