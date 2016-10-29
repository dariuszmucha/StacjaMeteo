function myFunction() {

     var findRestaurants = function(db, callback) {
        var cursor = db.collection('StacjaMeteo').find( ).limit(2);
        cursor.each(function(err, doc) {
           assert.equal(err, null);
           if (doc !== null) {

           } else {
              callback();
           }
        });
     };

    var MongoClient = require('mongodb').MongoClient;
    var assert = require('assert');
    alert('Hello, World!');

    var url = 'mongodb://ramboPower:QAZXSW1200@ds023425.mlab.com:23425/rambodb';


    MongoClient.connect(url, function(err, db) {
      assert.equal(null, err);
      findRestaurants(db, function() {
          db.close();
      });
    });
}

// o6k+EtSWiE+,#Z!
