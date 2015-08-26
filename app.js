var express = require('express');
var app = express();
var http = require('http').Server(app);
var path = require('path');

var io = require('socket.io')(http);
var usernames={};

app.use(express.static('/'));
app.use(express.static('views'));
app.use(express.static('public'));

app.get('/', function(req, res){
  res.sendFile('index.html');
});

io.on('connection', function(socket){
	var address = socket.handshake.address;
	console.log(address);
  console.log('New user connected.');

  //-----------------------------------------------

  var zmq = require('zmq');
  console.log("Connecting to ZMQ-node server...");
  var requester = zmq.socket('req');

  requester.on("message", function(reply){
    //console.log(reply.toString());
    var x = reply.toString().split(":");
    switch (x[0]){
      case "0":
        console.log("[disconnect]-----------------------(REP)");
        console.log(x[2]+"-->"+x[1]);
        break;
      case "1":
        //  "1" and "4"
        console.log("[createData/queryData]-----------------------(REP)");
        console.log(x);
        socket.emit('created', x);
        break;
      case "2":
        console.log("[login]-----------------------(REP)");
        console.log(x[2]+"-->"+x[1]);
        socket.emit('loginSuccess', x);
        break;
      case "3":
        console.log("[cloneData]-----------------------(REP)");
        console.log("clone.."+x[2]+" to "+x[3]+"-->"+x[1]);
        socket.emit('cloned', x);
        break;
      case "4":
        console.log("[getDatalist]-----------------------(REP)");
        console.log(x);
        socket.emit('onDatalists', x);
        break;
      case "5":
        console.log("[editData/getDatafromDatalist]-----------------------(REP)");
        console.log(x);
        socket.emit('genDatafromDatalists', x);
        break;
      case "6":
        console.log("[deleteData]-----------------------(REP)");
        console.log(x);
        socket.emit('deleteCallback', x);
        break;
      case "404":
        console.log("[addEdge/notification]-----------------------(REP)");
        console.log(x);
        socket.emit('notification', x[1]);
        break;
      case "10":
        console.log("[readmsg]-----------------------(REP)");
          if(x[2]=="none"){
            console.log('fetch by..'+x[1]+'\nno msg');
          }
          else{
            console.log('fetch by..'+x[1]+"\nfrom: "+x[2]+"\nto: "+x[3]+"\nmsg: "+x[4]);
              //+"\ntime: "+x[5]);
            console.log(socket.username);
            io.emit('fetchmsg',x[1],x[2],x[3],x[4]);
          }
          break;
    }
  });

requester.connect("tcp://localhost:6666");
console.log("Sending request...");

//  read msg
    setInterval(function(){
      // console.log("setinterval: 3");
      console.log(usernames);
      for(var key in usernames){
        console.log("[readmsg]-----------------------(REQ)");
        requester.send("10:"+key);
      }
    },4000);

  //-----------------------------------------------

  socket.on("createData",function(data){
    console.log("[createData]-----------------------(REQ)");
    console.log(data);
    if (data.msg == "1") {
      requester.send("1:"+data.msg+":"+data.name+":org");
    }
    else{
      requester.send("1:"+data.msg+":"+data.name+":"+data.upperid);
    }
    /*
    if (data.msg == "1") {
      requester.send("1:1:"+data.name);
    }
    else if (data.msg == "2") {
      requester.send("1:2:"+data.name);
    }
    else if (data.msg == "3") {
      requester.send("1:3:"+data.name);
    }
    else if (data.msg == "4") {
      requester.send("1:4:"+data.name);
    }
    else if (data.msg == "5") {
      requester.send("1:5:"+data.name);
    }
    else if (data.msg == "6") {
      requester.send("1:6:"+data.name);
    }
    else if (data.msg == "7") {
      requester.send("1:7:"+data.name);
    }
   */
 });

  socket.on("editData",function(data){
    console.log("[editData]-----------------------(REQ)");
    requester.send("2:"+data.id+":"+data.content);
  });

  socket.on("deleteData",function(data){
    console.log("[deleteData]-----------------------(REQ)");
    // requester.send("3:"+data);
    requester.send("3:"+data.objId+":"+data.upperId+":"+data.userId);
  });

  socket.on("queryData",function(data){
    console.log("[queryData]-----------------------(REQ)");
    console.log(data);
    requester.send("4:"+data);
  });

  socket.on("cloneData",function(data){
    console.log("[cloneData]-----------------------(REQ)");
    console.log("id: "+data.id+"\tsender: "+data.sender+"\ttarget: "+data.target);
    requester.send("5:"+data.id+":"+data.sender+":"+data.target);
  });

  socket.on("login",function(username){
    console.log("[login]-----------------------(REQ)");
    console.log(username);
    socket.username = username;
    usernames[username] = username;
    console.log(usernames);
    requester.send("6:"+username);
  });

  socket.on("getDatalist",function(data){
    console.log("[getDatalist]-----------------------(REQ)");
    if (data.msg == "8") {
      requester.send("7:8:org");
    }
    else{
      requester.send("7:"+data.msg+":"+data.id);
    }
  });

  socket.on("getDatafromDatalist",function(data){
    console.log("[getDatafromDatalist]-----------------------(REQ)");
    console.log(data);
    requester.send("8:"+data);
  });

  socket.on("addEdge",function(data){
    console.log("[addEdge]-----------------------(REQ)");
    requester.send("9:"+data.msg+":"+data.Ldata+":"+data.Rdata);
  }); 

  socket.on("disconnect",function(){
    if(usernames[socket.username] != undefined){
      console.log("[disconnect]-----------------------(REQ)");
      console.log(socket.username);
      console.log('\''+socket.username+'\' disconnected.');

      var tempname = socket.username;
      delete usernames[tempname];
      console.log(usernames);
      requester.send("0:"+tempname);
    }
    // else {
    //   console.log("undefined user");
    // }
  });
});

http.listen(3333, function(){
  console.log('listening on *:3333');
});
