var Id2append = "";
var currentId = "";
var createdId_2append = "";
var newWay = 0;
var separateDisplay = 0;
var myname="";

var socket = io.connect('http://localhost:3333');

socket.on('connect', function() {
  console.log("user conected")
});

socket.on('created', function(data) {
  console.log(data);
  var type = data[1];
  var listID = data[2];
  var listName = data[3];
  var parent = "";
  var code = "";
  
  var appendpoint = "";

  if (type == "Org") {
    parent = "orgHead-";
    appendpoint = "orgHead";
    title = "Organize";
    code = "1";
  }
  else if (type == "User") {
    parent = "userHead-";
    appendpoint = "orgHead-"+createdId_2append;
    title = "User";
    code = "2";
  }
  else if (type == "Category") {
    parent = "categoryHead-";
    appendpoint = "userHead-"+createdId_2append;
    title = "Category";
    code = "3";
  }
  else if (type == "State") {
    parent = "stateHead-";
    appendpoint = "categoryHead-"+createdId_2append;
    title = "State";
    code = "4";
  }
  else if (type == "Task") {
    if (newWay == 1) {
      parent = "taskHead2-";
      appendpoint = "categoryHead-"+createdId_2append;
      code = "9";
      newWay = 0;
    }
    else{
      parent = "taskHead-";
      appendpoint = "stateHead-"+createdId_2append;
      code = "5";
    }
    title = "Task";
  }
  else if (type == "Subtask") {
    if (newWay == 1) {
      parent = "subtaskHead2-";
      appendpoint = "taskHead2-"+createdId_2append;
      code = "10";
      newWay = 0;
    }
    else{
      parent = "subtaskHead-";
      appendpoint = "taskHead-"+createdId_2append;
      code = "8";
    }
    title = "Subtask";
  }   
  
  var child = "";
  var child2 = "";

  if (type == "Subtask") {
    child2 += "<li class='new-create'><a class='cursor' id='a-"+listID+"' onclick=\"query("+code+", '', this)\">"+listName+"</a>";
  }
  else{
    child2 += "<li class='new-create' id='data-"+listID+"'><a class='cursor' id='a-"+listID+"' onclick=\"query(" + code + ',\'' + listID +"', this)\">"+listName+"</a>";
  }
  child2 += "<ul id='"+parent+listID+"'></ul>";
  child2 += "</li>";

  // if input form manual query & clone
  if (separateDisplay == 1) {
    child += "<hr>";
    child += "<li><a>"+title+"</a>";  
    child += "<ul id='query-"+listID+"'>";
    child += "</ul>";
    child += "</li>";

    $("#querydisplay").append(child);
    $("#query-"+listID).append(child2);
    $("#querydisplay").treemenu({delay:200}).openActive();
    separateDisplay = 0 ;
  }
  else{
    if (type != "Org") {
      child += "<li class='new-create'><a>"+title+"</a>"; 
      child += "<ul id='"+parent+createdId_2append+"'>";
      child += "</ul>";
      child += "</li>"; 

      $("#"+appendpoint).append(child);
      $("#"+parent+createdId_2append).append(child2);
      // fix not necessary title
      var lastTitle = $("#"+appendpoint).children().last().children().first().text();
      var prevlastTitle = $("#"+appendpoint).children().last().prev().children().next().first().text()
      if (lastTitle == prevlastTitle) {
        $("#"+appendpoint).children().last().remove();
      }
    }
    else{
      $("#"+appendpoint).append(child2);
    }
  }
  alert("Create "+type+" "+listName+" success");
});

socket.on('loginSuccess', function(data) {
  // alert(data[1]);
  alert('\''+data[2]+'\' '+data[1]);
});

socket.on('cloned', function(data) {
  // alert(data[1]);
  alert("clone.."+x[2]+" to "+x[3]+"-->"+x[1]);
});

socket.on('fetchmsg',function(forme,fromuser,touser,msg) {
  if(forme==myname && touser==myname){
    alert("you received \'"+msg+"\' from "+fromuser);
  }
});

socket.on('notification', function(data) {
  alert(data);
});

socket.on('deleteCallback', function(data) {
  $("#data-"+data[2]).remove();
  alert(data[1]+" id: "+data[2]);
});

socket.on('onDatalists', function(data) {
  console.log(data);
  var len = (data.length-3)/2;
  var type = data[1];
  var parent = "";
  var code = "";
  var arr = [2, 3];
  var listID = "";
  var listName = "";
  var appendpoint = "";
  var title = "";

  if (type == "Org") {
    parent = "orgHead-";
    appendpoint = "#orgHead";
    code = "1";
  }
  else if (type == "User") {
    parent = "userHead-";
    appendpoint = "#orgHead-"+Id2append;
    title = "User";
    code = "2";
  }
  else if (type == "Category") {
    parent = "categoryHead-";
    appendpoint = "#userHead-"+Id2append;
    title = "Category";
    code = "3";
  }
  else if (type == "State") {
    parent = "stateHead-";
    appendpoint = "#categoryHead-"+Id2append;
    title = "State";
    code = "4";
  }
  else if (type == "Task") {
    parent = "taskHead-";
    appendpoint = "#stateHead-"+Id2append;
    title = "Task";
    code = "5";
  }
  else if (type == "Subtask") {
    parent = "subtaskHead-";
    appendpoint = "#taskHead-"+Id2append;
    title = "Subtask";
    code = "8";
  }    
  else if (type == "TaskfromCat"){
    parent = "taskHead2-";
    appendpoint = "#categoryHead-"+Id2append;
    title = "Task";
    code = "9";
  }
  else if (type == "SubtaskfromCat"){
    parent = "subtaskHead2-";
    appendpoint = "#taskHead2-"+Id2append;
    title = "Subtask";
    code = "10";
  }
  
  for (var i = 1; i <= len; i++) {
    if (i == 1) {
      listID = data[arr[0]];
      listName = data[arr[1]];
    }
    else{     
      arr[0] = arr[0]+2;
      arr[1] = arr[1]+2;

      listID = data[arr[0]];
      listName = data[arr[1]];
    }

    var child = "";
    var child2 = "";

    if (type == "Subtask") {
      child2 += "<li><a class='cursor' id='a-"+listID+"' onclick=\"query("+code+", '', this)\">"+listName+"</a>";
    }
    else{
      child2 += "<li id='data-"+listID+"'><a class='cursor' id='a-"+listID+"' onclick=\"query(" + code + ',\'' + listID +"', this)\">"+listName+"</a>";
    }
    child2 += "<ul id='"+parent+listID+"'></ul>";
    child2 += "</li>";


    if (type != "Org") {
      if (i == 1) {
        child += "<li><a>"+title+"</a>";  
        child += "<ul id='"+parent+currentId+"'>";
        child += "</ul>";
        child += "</li>"; 
      }
      console.log("--> #"+parent+listID);

      $(appendpoint).append(child);
      $("#"+parent+currentId).append(child2);
    }
    else{
      $(appendpoint).append(child2);
    }
  }

  // gen tree UI
  if (type == "Org") {
    $(appendpoint).treemenu({delay:200}).openActive();
  }
});

socket.on('genDatafromDatalists', function(data) {
  console.log(data);
  var dataName = data[2];
  var dataID = data[3];
  var dataType = data[4];
  var chatRoom = data[5];
  var verKeeped = data[6];
  var userHolder = "A, B, C";

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

  var type = data[1];
  var appendpoint = "";

  var dataLen = (data.length-8)/12;
  var arr = [7,8,9,10,11,12,13,14,15,16,17,18];

  if (type == "Org") {
    appendpoint = "orgHead-"+currentId;
  }
  else if (type == "User") {
    appendpoint = "userHead-"+currentId;
  }
  else if (type == "Category") {
    appendpoint = "categoryHead-"+currentId;
  }
  else if (type == "State") {
    appendpoint = "stateHead-"+currentId;
  }
  else if (type == "Task") {
    if (newWay == 1) {
      appendpoint = "taskHead2-"+currentId;
      newWay = 0;
    }
    else{
      appendpoint = "taskHead-"+currentId;
    }
  }
  else if (type == "Subtask") {
    if (newWay == 1) {
      appendpoint = "subtaskHead2-"+currentId;
      newWay = 0;
    }
    else{
      appendpoint = "subtaskHead-"+currentId;
    }
  }   

  // Create to DataHolder
  $("#"+appendpoint).removeClass("none");
  var parent = $("#"+appendpoint);
  var child = "";

  child += "<li><a class='dataid'>dataID : "+dataID+"</a></li>";
  child += "<li><a>dataType : "+dataType+"</a></li>";
  child += "<li>";
  child += "<a id='chatroom'>Chatroom : "+chatRoom+"</a>";
  child += "</li>";
  child += "<li><a>UserHolder : "+userHolder+"</a></li>";
  child += "<li><a>DataHolder</a>";
  child += "<ul id='dataHolder-"+dataID+"'>";
  child += "<li><a>versionKeeped : "+verKeeped+"</a></li>";

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
  var child2 = "<li><a>DataContent</a>";
  child2 += "<ul>";
  child2 += "<li><a>SHA256 : "+SHA256+"</a></li>";
  child2 += "<li><a>Timestamp : "+timestamp+"</a></li>";
  child2 += "<li><a>isDiff : "+isdiff+"</a></li>";
  child2 += "<li id='fulldata-"+dataID+"'><a>Full</a>";
  child2 += "<ul>";
  child2 += "<li><a>SchemaCode : "+full1+"</a></li>";
  child2 += "<li><a>byteCount : "+full2+"</a></li>";
  if (i == dataLen) {
    child2 += "<li>data : <xmp>"+full3+"</xmp><button type='button' class='btn btn-default btn-xs' data-toggle='modal' data-target='#editData' onclick=\"getdata2modal('" + dataID + "', this)\">edit</button></li>";
  }
  else{
    child2 += "<li>data : <xmp>"+full3+"</xmp></li>";
  }
  child2 += "</ul>";
  child2 += "</li>";
  child2 += "<li id='minusdata-"+dataID+"'><a>Minus</a>";
  child2 += "<ul>";
  child2 += "<li><a>SchemaCode : "+minus1+"</a></li>";
  child2 += "<li><a>byteCount : "+minus2+"</a></li>";
  child2 += "<li>data : <xmp>"+minus3+"</xmp></li>";
  child2 += "</ul>";
  child2 += "</li>";
  child2 += "<li id='plusdata-"+dataID+"'><a>Plus</a>";
  child2 += "<ul>";
  child2 += "<li><a>SchemaCode : "+plus1+"</a></li>";
  child2 += "<li><a>byteCount : "+plus2+"</a></li>";
  child2 += "<li>data : <xmp>"+plus3+"</xmp></li>";
  child2 += "</ul>";
  child2 += "</li>";
  child2 += "</ul>";
  child2 += "</li>";

  parent2.append(child2);
}
  // gen tree UI
  if (type == "Category") {
    setTimeout(function(){
      $("#"+appendpoint).treemenu({delay:200}).openActive();
    }, 200);
  }
  else{
    $("#"+appendpoint).treemenu({delay:200}).openActive();
  }
});

function create(val){
  var name = $("#dataName").val();
  if ($("#upperId").length) {
    var upperid = $("#upperId").val();
    createdId_2append = upperid;
  }
  if (name && !upperid) {
    if (val == 1) {
      // create Org
      socket.emit('createData', {msg: "1", name: name});
    }
    else{
      alert("Please fill in all 'Create data' fields");
    }
  }
  else if (name && upperid){
    if (val == 2) {
      // create User
      socket.emit('createData', {msg: "2", name: name, upperid: upperid});
    }
    else if (val == 3) {
      // create Category
      socket.emit('createData', {msg: "3", name: name, upperid: upperid});
    }
    else if (val == 4) {
      // create state
      socket.emit('createData', {msg: "4", name: name, upperid: upperid});
    }
    else if (val == 5) {
      // create Task
      socket.emit('createData', {msg: "5", name: name, upperid: upperid});
    }
    else if (val == 6) {
      // create Subtask
      socket.emit('createData', {msg: "6", name: name, upperid: upperid});
    }
    else if (val == 7) {
      // create Task2
      socket.emit('createData', {msg: "7", name: name, upperid: upperid});
      newWay = 1;
    }
    else if (val == 8) {
      // create Subtask2
      socket.emit('createData', {msg: "6", name: name, upperid: upperid});
      newWay = 1;
    }
  }
  else{
    alert("Please fill in all 'Create data' fields");
  } 
}

function deleteData(){
  var objId = $("#objectId").val();
  var upperId = $("#deleteUpperId").val();
  var userId = $("#userUpperId").val();
  if (objId && upperId && userId) {
    socket.emit('deleteData', {objId: objId, upperId: upperId, userId: userId});
  }
  else{
    alert("Please fill in all 'Delete data' fields");
  }
}

function login2transport(){
  var name = $("#loginName").val();
  myname = name; 
  if (name) {
    socket.emit('login', name);
    $("#loginName").val("Login as "+name);
    $("#loginName").attr("disabled", "disabled");
    $(".loginBox button").attr("disabled", "disabled");
  }
  else{
    alert("Please fill yourname");
  }  
  
}

function editDatacontent(){
  var content = $("#edit-dataContent").val();
  var dataid = $("#edit-dataId").text();
  if (content && dataid) {
    socket.emit('editData', {id: dataid, content: content});
    $("#data-"+currentId).children().last().empty();
    $('#editData').modal('hide');
  }
  else{
    alert("Please fill in all 'Edit data' fields");
  }
}

function cloneData(){
  var dataid = $("#selectId").val();
  var toUsername = $("#selectUsername").val();
  if (dataid && toUsername) {
    alert(dataid+":"+myname+":"+toUsername);
    // socket.emit('cloneData', {id: dataid, user: toUsername});
    socket.emit('cloneData', {id: dataid, sender: myname, target: toUsername});
    separateDisplay = 1;
  }
  else{
    alert("Please fill in all 'Clone data' fields");
  }
  
}

function manualQueryData(){
  var dataid = $("#queryId").val();
  if (dataid) {
    socket.emit('queryData', dataid);
    separateDisplay = 1;
  }
  else{
    alert("Please input dataID");
  }
}

function query(val, id, ele){
  if (val == 1) {
    // Get User lists
    if ($(ele).hasClass("open")) {
      $(ele).removeClass("open");
      $(ele).next().empty();
    }
    else{
      $(ele).addClass("open");

      currentId = $(ele).attr('id').slice(-32);
      console.log("currentId: "+currentId);
      Id2append = id;
      socket.emit('getDatalist', {msg: "1", id: id});
      socket.emit('getDatafromDatalist', currentId);
    }
  }
  else if (val == 2) {
    // Get Category lists
    if ($(ele).hasClass("open")) {
      $(ele).removeClass("open");
      $(ele).next().empty();
    }
    else{
      $(ele).addClass("open");

      currentId = $(ele).attr('id').slice(-32);
      console.log("currentId: "+currentId);
      Id2append = id; 
      socket.emit('getDatalist', {msg: "2", id: id});
      socket.emit('getDatafromDatalist', currentId);
    }
   }
   else if (val == 3) {
    // Get State lists
    if ($(ele).hasClass("open")) {
      $(ele).removeClass("open");
      $(ele).next().empty();
    }
    else{
      $(ele).addClass("open");

      currentId = $(ele).attr('id').slice(-32);
      console.log("currentId: "+currentId);
      Id2append = id;
      socket.emit('getDatalist', {msg: "3", id: id});
      socket.emit('getDatafromDatalist', currentId);
      query(6, id, ele);
    }
   }
   else if (val == 4) {
    // Get Task from State lists
    if ($(ele).hasClass("open")) {
      $(ele).removeClass("open");
      $(ele).next().empty();
    }
    else{
      $(ele).addClass("open");

      currentId = $(ele).attr('id').slice(-32);
      console.log("currentId: "+currentId);
      Id2append = id;
      socket.emit('getDatalist', {msg: "4", id: id});
      socket.emit('getDatafromDatalist', currentId);
    }
  }
  else if (val == 5) {
    // Get Subtask lists
    if ($(ele).hasClass("open")) {
      $(ele).removeClass("open");
      $(ele).next().empty();
    }
    else{
      $(ele).addClass("open");

      currentId = $(ele).attr('id').slice(-32);
      console.log("currentId: "+currentId);
      Id2append = id;
      socket.emit('getDatalist', {msg: "5", id: id});
      socket.emit('getDatafromDatalist', currentId);
    }
  }
  else if (val == 6) {
    // Get Task from Category lists
    currentId = $(ele).attr('id').slice(-32);
    console.log("currentId: "+currentId);
    Id2append = id;
    socket.emit('getDatalist', {msg: "6", id: id});

  }
  else if (val == 7) {
    // Get Org lists
    if ($(ele).hasClass("open")) {
      $(ele).removeClass("open");
      $(ele).next().empty();
    }
    else{
      $(ele).addClass("open");
      socket.emit('getDatalist', {msg: "8"});
    }
  }
  else if (val == 8) {
    // Get Subtask data
    if ($(ele).hasClass("open")) {
      $(ele).removeClass("open");
      $(ele).next().empty();
    }
    else{
      $(ele).addClass("open");

      currentId = $(ele).attr('id').slice(-32);
      console.log("currentId: "+currentId);
      Id2append = id;
      socket.emit('getDatafromDatalist', currentId);
    }
  }
  else if (val == 9) {
    // Get Subtasklists from Category lists
    if ($(ele).hasClass("open")) {
      $(ele).removeClass("open");
      $(ele).next().empty();
    }
    else{
      $(ele).addClass("open");

      currentId = $(ele).attr('id').slice(-32);
      console.log("currentId: "+currentId);
      Id2append = id;
      socket.emit('getDatalist', {msg: "7", id: id});
      socket.emit('getDatafromDatalist', currentId);

      newWay = 1;
    }
  }
  else if (val == 10) {
    // Get Subtask data from Category 
    if ($(ele).hasClass("open")) {
      $(ele).removeClass("open");
      $(ele).next().empty();
    }
    else{
      $(ele).addClass("open");

      currentId = $(ele).attr('id').slice(-32);
      console.log("currentId: "+currentId);
      Id2append = id;
      socket.emit('getDatafromDatalist', currentId);

      newWay = 1;
    }
  }
}

function addEdge(val){
  var Ldata = $("#Ldata").val();
  var Rdata = $("#Rdata").val();
  if (Ldata && Rdata) {
    if (val == 1){
      socket.emit('addEdge', {msg: "1", Ldata: Ldata, Rdata: Rdata});
    }
    else if (val == 2){
      socket.emit('addEdge', {msg: "2", Ldata: Ldata, Rdata: Rdata});
    }
    else if (val == 3){
      socket.emit('addEdge', {msg: "3", Ldata: Ldata, Rdata: Rdata});
    }
    else if (val == 4){
      socket.emit('addEdge', {msg: "4", Ldata: Ldata, Rdata: Rdata});
    }
    else if (val == 5){
      socket.emit('addEdge', {msg: "5", Ldata: Ldata, Rdata: Rdata});
    }
    else if (val == 6){
      socket.emit('addEdge', {msg: "6", Ldata: Ldata, Rdata: Rdata});
    }
  }
  else{
    alert("Please fill in all 'Add edge' fields");
  }
}

function openform(val){
  if ($(val).next("div").hasClass("none")){
    $(val).next("div").removeClass("none");
  }
  else{
    $(val).next("div").addClass("none");
  }
}

function getdata2modal(id, ele){
  var content = $(ele).prev().text();
  $("#edit-dataContent").val(content);
  $("#edit-dataId").html(id);
}

function createUI(ele) {
  var val = $(ele).val(); 
  var child = "";
  if (val == "org"){
    child += "<input class='form-control inline input-sm custom-size' id='dataName' type='text' placeholder='DataName'>";
    child += "<button type='button' class='btn btn-default btn-sm' onclick='create(1)'>Create</button>";
  }
  else if (val == "user"){
    child += "<input class='form-control inline input-sm custom-size' id='dataName' type='text' placeholder='DataName'>";
    child += "<input class='form-control inline input-sm id-input' id='upperId' type='text' placeholder='OrgID'>";
    child += "<button type='button' class='btn btn-default btn-sm' onclick='create(2)'>Create</button>";  
  }
  else if (val == "category"){
    child += "<input class='form-control inline input-sm custom-size' id='dataName' type='text' placeholder='DataName'>";
    child += "<input class='form-control inline input-sm id-input' id='upperId' type='text' placeholder='UserID'>";
    child += "<button type='button' class='btn btn-default btn-sm' onclick='create(3)'>Create</button>";  
  }
  else if (val == "state"){
    child += "<input class='form-control inline input-sm custom-size' id='dataName' type='text' placeholder='DataName'>";
    child += "<input class='form-control inline input-sm id-input' id='upperId' type='text' placeholder='CategoryID'>";
    child += "<button type='button' class='btn btn-default btn-sm' onclick='create(4)'>Create</button>";  
  }
  else if (val == "task"){
    child += "<input class='form-control inline input-sm custom-size' id='dataName' type='text' placeholder='DataName'>";
    child += "<input class='form-control inline input-sm id-input' id='upperId' type='text' placeholder='StateID'>";
    child += "<button type='button' class='btn btn-default btn-sm' onclick='create(5)'>Create</button>";  
  }
  else if (val == "subtask"){
    child += "<input class='form-control inline input-sm custom-size' id='dataName' type='text' placeholder='DataName'>";
    child += "<input class='form-control inline input-sm id-input' id='upperId' type='text' placeholder='TaskID'>";
    child += "<button type='button' class='btn btn-default btn-sm' onclick='create(6)'>Create</button>";  
  }
  else if (val == "task2"){
    child += "<input class='form-control inline input-sm custom-size' id='dataName' type='text' placeholder='DataName'>";
    child += "<input class='form-control inline input-sm id-input' id='upperId' type='text' placeholder='CategoryID'>";
    child += "<button type='button' class='btn btn-default btn-sm' onclick='create(7)'>Create</button>";  
  }
  else if (val == "subtask2"){
    child += "<input class='form-control inline input-sm custom-size' id='dataName' type='text' placeholder='DataName'>";
    child += "<input class='form-control inline input-sm id-input' id='upperId' type='text' placeholder='TaskID'>";
    child += "<button type='button' class='btn btn-default btn-sm' onclick='create(8)'>Create</button>";  
  }
  $(".createForm").empty();
  $(".createForm").append(child);
}

function addEdgeUI(ele){
  var val = $(ele).val(); 
  var child = "";
  if (val == 1){
    child += "<input class='form-control inline input-sm id-input' id='Ldata' type='text' placeholder='UserID'>";
    child += "<input class='form-control inline input-sm id-input' id='Rdata' type='text' placeholder='OrgID'>";
    child += "<button type='button' class='btn btn-default btn-sm' onclick='addEdge(1)'>Add</button>";
  }
  else if (val == 2){
    child += "<input class='form-control inline input-sm id-input' id='Ldata' type='text' placeholder='CategoryID'>";
    child += "<input class='form-control inline input-sm id-input' id='Rdata' type='text' placeholder='UserID'>";
    child += "<button type='button' class='btn btn-default btn-sm' onclick='addEdge(2)'>Add</button>";
  }
  else if (val == 3){
    child += "<input class='form-control inline input-sm id-input' id='Ldata' type='text' placeholder='StateID'>";
    child += "<input class='form-control inline input-sm id-input' id='Rdata' type='text' placeholder='CategoryID'>";
    child += "<button type='button' class='btn btn-default btn-sm' onclick='addEdge(3)'>Add</button>";
  }
  else if (val == 4){
    child += "<input class='form-control inline input-sm id-input' id='Ldata' type='text' placeholder='TaskID'>";
    child += "<input class='form-control inline input-sm id-input' id='Rdata' type='text' placeholder='StateID'>";
    child += "<button type='button' class='btn btn-default btn-sm' onclick='addEdge(4)'>Add</button>";
  }
  else if (val == 5){
    child += "<input class='form-control inline input-sm id-input' id='Ldata' type='text' placeholder='SubtaskID'>";
    child += "<input class='form-control inline input-sm id-input' id='Rdata' type='text' placeholder='TaskID'>";
    child += "<button type='button' class='btn btn-default btn-sm' onclick='addEdge(5)'>Add</button>";
  }
  else if (val == 6){
    child += "<input class='form-control inline input-sm id-input' id='Ldata' type='text' placeholder='TaskID'>";
    child += "<input class='form-control inline input-sm id-input' id='Rdata' type='text' placeholder='CategoryID'>";
    child += "<button type='button' class='btn btn-default btn-sm' onclick='addEdge(6)'>Add</button>";
  }
  $(".edgeForm").empty();
  $(".edgeForm").append(child);
}

function clearSeparatedisplay(){
  $("#querydisplay").empty();
}

$(document).ready(function(){
  $(".tree").treemenu({delay:200}).openActive();
});