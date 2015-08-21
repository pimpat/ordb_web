var dataName = "";
var dataID = "";
var dataType = "";
var chatRoom = "";
var verKeeped = "";
var SHA256 = "";
var timestamp = "";
var isdiff = "";

var full1 = "";
var full2 = "";
var full3 = "";
var minus1 = "";
var minus2 = "";
var minus3 = "";
var plus1 = "";
var plus2 = "";
var plus3 = "";

var newWay = 0;
var deleteable = 0;

var socket = io.connect('http://localhost:3333');

var myname="";

socket.on('connect', function() {
  console.log("user conected")
});

socket.on('created', function(data) {
  dataName = data[2];
  dataID = data[3];
  dataType = data[4];
  chatRoom = data[5];
  verKeeped = data[6];

  var type = data[1];
  var appendpoint = "";

  var dataLen = (data.length-8)/12;
  var arr = [7,8,9,10,11,12,13,14,15,16,17,18];

  if (type == "Org") {
    appendpoint = "orgHead";
  }
  else if (type == "User") {
    appendpoint = "userHead";
  }
  else if (type == "Category") {
    appendpoint = "categoryHead";
  }
  else if (type == "State") {
    appendpoint = "stateHead";
  }
  else if (type == "Task") {
    if (newWay == 1) {appendpoint = "taskHead2"; newWay = 0;}
    else {appendpoint = "taskHead";}
    deleteable = 1;
  }
  else if (type == "Subtask") {
    if (newWay == 1) {appendpoint = "subtaskHead2"; newWay = 0;}
    else {appendpoint = "subtaskHead";}
    deleteable = 1;
  }    

  // Create to DataHolder
  $("#"+appendpoint).removeClass("none");
  var parent = $("#"+appendpoint);
  var child = "";
  child += "<li id='data-"+dataID+"'><a href='#' id='dname'>"+dataName+"</a>";
  if (deleteable == 1) {
    child += "<button type='button' class='btn btn-danger btn-xs' onclick=\"deleteData('" + dataID + "')\">delete</button>";
    deleteable = 0;
  }   
  //child += "<button type='button' class='btn btn-primary btn-xs' onclick=\"clone('" + dataID + "')\">clone</button>";
  child += "<ul>";
  child += "<li><a href='#'>dataID : "+dataID+"</a></li>";
  child += "<li><a href='#'>dataType : "+dataType+"</a></li>";
  child += "<li>";
  child += "<a href='#' id='chatroom'>Chatroom : "+chatRoom+"</a>";
  //child += "<button type='button' class='btn btn-default btn-xs' data-toggle='modal' data-target='#editData' onclick=\"getdata2modal('" + dataID + '\',\'' + full3 +"')\">edit</button>";
  child += "</li>";
  child += "<li><a href='#'>DataHolder</a>";
  child += "<ul id='dataHolder-"+dataID+"'>";
  child += "<li><a href='#'>versionKeeped : "+verKeeped+"</a></li>";

  child += "</ul>";
  child += "</li>";
  child += "</ul>";
  child += "</li>";
  child += "</ul>";
  child += "</li>";
  parent.prepend(child);

  for (var i = 1; i <= dataLen; i++) {
        // Create each DataContent
        if (i == 1) {
          SHA256 = data[arr[0]];
          timestamp = data[arr[1]];
          isdiff = data[arr[2]];
          full1 = data[arr[3]];
          full2 = data[arr[4]];
          full3 = data[arr[5]];
          minus1 = data[arr[6]];
          minus2 = data[arr[7]];
          minus3 = data[arr[8]];
          plus1 = data[arr[9]];
          plus2 = data[arr[10]];
          plus3 = data[arr[11]];
        }
        else{     
          arr[0] = arr[0]+12;
          arr[1] = arr[1]+12;
          arr[2] = arr[2]+12;
          arr[3] = arr[3]+12;
          arr[4] = arr[4]+12;
          arr[5] = arr[5]+12;
          arr[6] = arr[6]+12;
          arr[7] = arr[7]+12;
          arr[8] = arr[8]+12;
          arr[9] = arr[9]+12;
          arr[10] = arr[10]+12;
          arr[11] = arr[11]+12;

          SHA256 = data[arr[0]];
          timestamp = data[arr[1]];
          isdiff = data[arr[2]];
          full1 = data[arr[3]];
          full2 = data[arr[4]];
          full3 = data[arr[5]];
          minus1 = data[arr[6]];
          minus2 = data[arr[7]];
          minus3 = data[arr[8]];
          plus1 = data[arr[9]];
          plus2 = data[arr[10]];
          plus3 = data[arr[11]];
        }

        var parent2 = $("#dataHolder-"+dataID);
        var child2 = "<li><a href='#''>DataContent</a>";
        child2 += "<ul>";
        child2 += "<li><a href='#'>SHA256 : "+SHA256+"</a></li>";
        child2 += "<li><a href='#'>Timestamp : "+timestamp+"</a></li>";
        child2 += "<li><a href='#'>isDiff : "+isdiff+"</a></li>";
        child2 += "<li id='fulldata-"+dataID+"'><a href='#'>Full</a>";
        child2 += "<ul>";
        child2 += "<li><a href='#'>SchemaCode : "+full1+"</a></li>";
        child2 += "<li><a href='#'>byteCount : "+full2+"</a></li>";
        if (i == dataLen) {
          child2 += "<li>data : <xmp>"+full3+"</xmp><button type='button' class='btn btn-default btn-xs' data-toggle='modal' data-target='#editData' onclick=\"getdata2modal('" + dataID + "', this)\">edit</button></li>";
        }
        else{
          child2 += "<li>data : <xmp>"+full3+"</xmp></li>";
        }
        child2 += "</ul>";
        child2 += "</li>";
        child2 += "<li id='minusdata-"+dataID+"'><a href='#'>Minus</a>";
        child2 += "<ul>";
        child2 += "<li><a href='#'>SchemaCode : "+minus1+"</a></li>";
        child2 += "<li><a href='#'>byteCount : "+minus2+"</a></li>";
        child2 += "<li>data : <xmp>"+minus3+"</xmp></li>";
        child2 += "</ul>";
        child2 += "</li>";
        child2 += "<li id='plusdata-"+dataID+"'><a href='#'>Plus</a>";
        child2 += "<ul>";
        child2 += "<li><a href='#'>SchemaCode : "+plus1+"</a></li>";
        child2 += "<li><a href='#'>byteCount : "+plus2+"</a></li>";
        child2 += "<li>data : <xmp>"+plus3+"</xmp></li>";
        child2 += "</ul>";
        child2 += "</li>";
        child2 += "</ul>";
        child2 += "</li>";

        parent2.append(child2);
      }
      // Create Tree UI
      $("#dataHolder-"+dataID).treemenu({delay:200}).openActive();
    });

socket.on('loginSuccess', function(data) {
  alert('\''+data[2]+'\' '+data[1]);
});

socket.on('cloned', function(data) {
  alert("clone.."+x[2]+" to "+x[3]+"-->"+x[1]);
});

socket.on('fetchmsg',function(forme,fromuser,touser,msg) {
  if(forme==myname && touser==myname){
    alert("you received \'"+msg+"\' from "+fromuser);
  }
});

// req(1);
// req(2);

function req(val){
  if (val == 1) {
    // create Org
    //var name = "name";
     var name = $("#orgInput").val();
    socket.emit('createData', {msg: "1", name: name});
  }
  else if (val == 2) {
    // create User
    //var name = "name";
     var name = $("#userInput").val();
    socket.emit('createData', {msg: "2", name: name});
  }
  else if (val == 3) {
    // create Category
    var name = $("#categoryInput").val();
    socket.emit('createData', {msg: "3", name: name});
  }
  else if (val == 4) {
    // create state
    var name = $("#stateInput").val();
    socket.emit('createData', {msg: "4", name: name});
  }
  else if (val == 5) {
    // create Task
    var name = $("#taskInput").val();
    socket.emit('createData', {msg: "5", name: name});
  }
  else if (val == 6) {
    // create Subtask
    var name = $("#subtaskInput").val();
    socket.emit('createData', {msg: "6", name: name});
  }
  else if (val == 7) {
    // create Task2
    var name = $("#taskInput2").val();
    socket.emit('createData', {msg: "7", name: name});
    newWay = 1;
  }
  else if (val == 8) {
    // create Subtask2
    var name = $("#subtaskInput2").val();
    socket.emit('createData', {msg: "6", name: name});
    newWay = 1;
  }
  else if (val == 9) {
    // saveDatacontent
    var content = $("#edit-dataContent").val();
    var dataid = $("#edit-dataId").text();
    socket.emit('editData', {id: dataid, content: content});
    $("#data-"+dataid).remove();
    $('#editData').modal('hide');
  }
  else if (val == 10) {
    // queryOrgFromData
    // alert("test");
    // alert($("#selectId").val()+":"+$("#selectUsername").val());
    socket.emit('queryData', "Hello");
  }
  else if (val == 11) {
    // Login
    var name = $("#loginName").val();
    myname = name;    
    socket.emit('login', name);
    $(".cloneBox").removeClass("none");
    $(".queryBox").removeClass("none");
    $("#loginName").val("Login as "+name);
    $("#loginName").attr("disabled", "disabled");
    $(".loginBox button").attr("disabled", "disabled");
  }
  else if (val == 12) {
    // Clone Data
    var dataid = $("#selectId").val();
    // var name = $("#loginName").val(); 
    var toUsername = $("#selectUsername").val();
    alert(dataid+":"+myname+":"+toUsername);

    socket.emit('cloneData', {id: dataid, sender: myname, target: toUsername});
  }
  else if (val == 13) {
    // Query Data
    var dataid = $("#queryId").val();
    socket.emit('queryData', dataid);
  }
}

function deleteData(id) {
  console.log(id);
  //socket.emit('deleteData', id);
}


function openform(val) {
  var selector =  "";
  if (val == "org") {
    selector = "orgForm";
  }
  else if (val == "user") {
    selector = "userForm";
  }
  else if (val == "category") {
    selector = "categoryForm";
  }
  else if (val == "state") {
    selector = "stateForm";
  }
  else if (val == "task") {
    selector = "taskForm";
  }
  else if (val == "subtask") {
    selector = "subtaskForm";
  }
  else if (val == "task2") {
    selector = "taskForm2";
  }
  else if (val == "subtask2") {
    selector = "subtaskForm2";
  }

  if ($("#"+selector).hasClass("none")) {
    $("#"+selector).removeClass("none");
  }
  else{
    $("#"+selector).addClass("none");
  }
}


function getdata2modal(id, ele) {
  var content = $(ele).prev().text();
  console.log(content);
  $("#edit-dataContent").val(content);
  $("#edit-dataId").html(id);
}