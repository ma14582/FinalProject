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
//websockets
var server = require('http').Server(express);
var io = require('socket.io')(server);

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
                            .select('pet.name as pet', 'event_type.name as event', 'event.date', 'event.time')
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
                            .select('pet.name as pet', 'event_type.name as event', 'event.date', 'event.time')
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

// Get barks alongside with their date and time order by date ascending according to a date range.
router.route('/test')
        .get(
                function (req, res) {
                    var result = {
                        "events": []
                    };
                    Event.query()
                            .select('pet.name as pet', 'event_type.name as event', 'event.date', 'event.time')
                            .join('pet', 'pet.id', '=', 'event.pet_id')
                            .join('event_type', 'event_type.id', '=', 'event.event_id')
                            .whereBetween('event.date', ['2010-01-01', '2010-12-31']) 
                            .orderBy('event.id')
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
                            .select('pet.name as pet', 'event_type.name as event', 'event.date', 'event.time')
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

// Insert a new sound alongside with the date and time
router.route('/insertEvent')
        .post(
                function (req, res) {                 
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
                
                
// websocket endpoint
io.on('connection', function (socket) {
    socket.emit('message', 
                {
                    username: 'cooper', 
                    type: 'BARK', 
                    date: '15-02-2020 19:20:00', 
                    duration: '15'
                });
    socket.on('update', function (data) {
        console.log(data);
    });
});

app.use('/', router);
app.listen(3000, function () {
    console.log("    Server started %d in %s mode", 3000, app.get('env'));
});

server.listen(3003);
