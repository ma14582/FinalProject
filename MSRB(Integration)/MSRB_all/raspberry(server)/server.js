/*
 ===============================================
 DB CONECTION
 ===============================================
 */

var knex = require('knex')({
    client: 'sqlite3',
    connection: {
        filename: "./database.sqlite"
    }
});

/*
 ===============================================
 DEPENDENCIES 
 ===============================================
 */

var Bookshelf = require('bookshelf')(knex);
var express = require('express');
var cors = require('cors');
var app = express();
var bodyParser = require('body-parser');
var _ = require('lodash');
var router = express.Router();
app.use(cors());
app.use(bodyParser.urlencoded({extended: true}));
app.use(bodyParser.json());
var Promise = require('bluebird');
var server = require('http').Server(express);

/*
 ===============================================
 MODEL / COLLECTIONS
 ===============================================
 */

//Model
var Pet = Bookshelf.Model.extend({
    tableName: 'pet'
});
var Event_Type = Bookshelf.Model.extend({
    tableName: 'event_type'
});
var Event = Bookshelf.Model.extend({
    tableName: 'event',
    who: function () {
        return this.belongsTo(Pet);
    },
    type: function () {
        return this.belongsTo(Event_Type);
    }
});

//Collections
var Pets = Bookshelf.Collection.extend({
    model: Pet
});
var Events_Type = Bookshelf.Collection.extend({
    model: Events_Type
});
var Events = Bookshelf.Collection.extend({
    model: Event
});

/*
 ===============================================
 SERVICES
 ===============================================
 */

// Get 10 events alongside with the pet name, their type, date and time order by date ascending.
router.route('/getEvent')
        .get(
                function (req, res) {
                    var result = {
                        "events": []
                    };
                    Event.query()
                            .select('event.id', 'pet.name', 'event_type.name as event', 'event.date', 'event.time')
                            .join('pet', 'pet.id', '=', 'event.pet_id')
                            .join('event_type', 'event_type.id', '=', 'event.event_id')
                            .orderBy('event.id', 'desc')
                            .limit(10)
                            .then(function (records) {
                                records.forEach(function (record) {
                                    result.events.push(record);
                                });
                                res.json(result);
                            });
                });

// Get 10 barks alongside with their date and time order by date ascending.
router.route('/getBark')
        .get(
                function (req, res) {
                    var result = {
                        "events": []
                    };
                    Event.query()
                            .select('event.id', 'pet.name', 'event_type.name as event', 'event.date', 'event.time')
                            .join('pet', 'pet.id', '=', 'event.pet_id')
                            .join('event_type', 'event_type.id', '=', 'event.event_id')
                            .where('event_type.name', '=', 'bark')
                            .orderBy('event.id', 'desc')
                            .limit(10)
                            .then(function (records) {
                                records.forEach(function (record) {
                                    result.events.push(record);
                                });
                                res.json(result);
                            });
                });

// Get 10 movements alongside with their date and time order by date ascending.
router.route('/getMovement')
        .get(
                function (req, res) {
                    var result = {
                        "events": []
                    };
                    Event.query()
                            .select('event.id', 'pet.name', 'event_type.name as event', 'event.date', 'event.time')
                            .join('pet', 'pet.id', '=', 'event.pet_id')
                            .join('event_type', 'event_type.id', '=', 'event.event_id')
                            .where('event_type.name', '=', 'movement')
                            .orderBy('event.id', 'desc')
                            .limit(10)
                            .then(function (records) {
                                records.forEach(function (record) {
                                    result.events.push(record);
                                });
                                res.json(result);
                            });
                });
// Get the last id inserted
router.route('/getLastEvent')
        .get(
                function (req, res) {
                    var result = {
                        "events": []
                    };
                    Event.query()
                            .select('event.id')
                            .orderBy('event.id', 'desc')
                            .limit(1)
                            .then(function (records) {
                                records.forEach(function (record) {
                                    result.events.push(record);
                                });
                                res.json(result);
                            });
                });
        

// Insert a new sound alongside with the date and time
router.route('/insertEvent')
        .get(
                function (req, res) {
                    var today = getToday();
                    var time = getTime();   

                    Event.forge({
                        pet_id : req.query.pet_id,
                        event_id : req.query.event_id,
                        date : today,
                        time : time
                    })
                    .save()
                    .then(function(event) {
                        console.log("Event Inserted!");
                        res.json({ result : 'OK'});
                    });
                });

// Getting the date  
function getToday() {
    var today = new Date();
    var dd = today.getDate();
    var mm = today.getMonth() + 1;
    var yyyy = today.getFullYear();
    if (dd < 10) {
        dd = '0' + dd;
    }
    if (mm < 10) {
        mm = '0' + mm;
    }
    return today = yyyy + '/' + mm + '/' + dd;
}

// Getting the time 
function getTime() {
    var today = new Date();
    var hh = today.getHours();
    var mm = today.getMinutes();
    var ss = today.getSeconds();
    if (hh < 10) {
        hh = '0' + hh;
    }
    if (mm < 10) {
        mm = '0' + mm;
    }
    if (ss < 10) {
        ss = '0' + ss;
    }
    return today = hh + ':' + mm + ':' + ss;
}

// Insert a new sound alongside with the date and time
router.route('/insertEventTest')
        .post(
                function (req, res) {
                    //console.log(req);               
                    Event.forge({
                        pet_id: req.body.petId,
                        event_id: req.body.eventId,
                        date: req.body.date,
                        time: req.body.time
                    })
                    .save()
                    .then(function (sound) {
                        res.json({data: {id: event.get('id')}});
                    })
                    .catch(function (err) {
                        res.status(500).json({error: {message: err.message}});
                    });
                });

app.use('/', router);
app.listen(3000, function () {
    console.log("    Server started %d in %s mode", 3000, app.get('env'));
});

server.listen(3003);
