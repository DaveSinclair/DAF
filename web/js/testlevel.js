function selectObjectAttributes(objectTypeSelectorId) {

      // Issue an asynchronous request to the server for a list of all object attributes for object of type objecttype
      // json data will be returned in the form:
      // [ { ID: "11", Name: "Model", ObjecttypeID: "27", Type: "varchar", Varcharwidth: "32, Enumvalues: ""}, ... ] 
      dojo.xhrGet({
		url: "rest/daf/objectattributetype",
		load: function(data, ioargs) {
		         objectTypeSelector = dojo.byId(objectTypeSelectorId);
		         var selectedObjectTypeID = objectTypeSelector.options[objectTypeSelector.selectedIndex].value;
		         /* remove any tr rows that have been added previously by this routine to the table */
		         /* rows 0 and 1 are reserved for the Objecttype and Name of the Object             */
		         table = dojo.byId("queryobjecttableid");
		         var items = table.getElementsByTagName("tr");   
		         var i = items.length - 1; 
                 while (i > 3) {
                    dojo.destroy(items[i]);
                    i--;    
                 } 
		         /* add a new tr row to the table for each object specific attribute */
		         for (i=0; i<data.length; i++) {
		            if (data[i]["ObjecttypeID"] == selectedObjectTypeID) {
		               var r = '<td><select name="and_or[' + (i+2) + ']" />' +
                                  '<option value="AND" selected>AND</option>' + 
                                  '<option value="OR">OR</option>' + 
                               '</select></td>' +
                               '<td>' + data[i]["Name"] + '</td>' + 
                               '<td><select name="comparator[' + (i+2) + ']" />' +
                                  '<option value="LIKE">LIKE</option>' +
                                  '<option value="LIKEPC">LIKE %..%</option>' +
                                  '<option value="NOTLIKE">NOT LIKE</option>' +
                                  '<option value="REGEXP">REGEXP</option>' +
                                  '<option value="NOTREGEXP">NOT REGEXP</option>' +
                                  '<option value="ISEMPTY">=\'\' (is empty string)</option>' +
                                  '<option value="ISNOTEMPTY">!=\'\' (is not an empty string)</option>' +
                                  '<option value="=" selected>=</option>' +
                                  '<option value="!=">&ne;</option>' +
                                  '<option value="ISNULL">IS NULL</option>' +
                                  '<option value="ISNOTNULL">IS NOT NULL</option>' +
                               '</select></td>' +
                               '<td><input type="text" name="value[' + (i+2) + ']" />' +
                               '<input type="hidden" name="field[' + (i+2) + ']" value="' + data[i]["Name"] + '"/></td>';
		               dojo.create("tr", {
		                             innerHTML: r    
		                           }, table, 'last');
		            }   
		         }
		      },
		error: function(data, ioargs) {
		         alert("An error occurred while invoking selectObjectAttributes()");     /* <<<<< need a better error handler */
		      },
		handleAs: 'json'
	  });	  

}

function showVarcharOrEnum1(typeSelectorId) {  /* NOT IN USE */

   var typeSelector = dojo.byId(typeSelectorId);
   var selectedType = typeSelector.options[typeSelector.selectedIndex].value;   /* will be 'varchar' or 'enum' */
   var table = dojo.byId("createobjectattributetableid");
   var val1 = dojo.byId("val1");

   var items = table.getElementsByTagName("tr");
  
   /* remote the tr row in the table for the varchar or enum input field */
   var i = items.length - 1; 
   while (i > 2) {
      dojo.destroy(items[i]);
      i--;    
   } 
   
   /* add a new tr row to the table for either the varchar or enum input field */
   if (selectedType == 'varchar') {
      //var r = '<th>Varchar width</th><td><input type="text" name="varcharwidth"/></td>';
      // dojo.create("tr", {innerHTML: r}, table, 'last');
      
      var v = new dijit.form.ValidationTextBox({
         // id: "varcharwidthid",
         regExp: "\\d+",
         promtMessage: "Enter varchar field with for Object Attribute Type",
         invalidMessage: "Invalid Object Attribute Varchar width (must be a decimal number)",
         trim: true,
         required: true });
         dojo.place(v, val1, "last");
   } else {
      var r = '<th>Enum values</th><td><input type="text" name="enumvalues"/></td>';
      dojo.create("tr", {innerHTML: r}, table, 'last');
   }

}

function showVarcharOrEnum3(typeSelectorId, val) {  /* NOT IN USE */

   var typeSelector = dojo.byId(typeSelectorId);
   var selectedType = typeSelector.options[typeSelector.selectedIndex].value;   /* will be 'varchar' or 'enum' */
   var table = dojo.byId("createobjectattributetableid");
   var val1 = dojo.byId("val2");

   var items = table.getElementsByTagName("tr");
  
   /* remote the tr row in the table for the varchar or enum input field */
   var i = items.length - 1; 
   while (i > 2) {
      dojo.destroy(items[i]);
      i--;    
   } 
   
   /* add a new tr row to the table for either the varchar or enum input field */
   if (selectedType == 'varchar') {
      //var r = '<th>Varchar width</th><td><input type="text" name="varcharwidth"/></td>';
      // dojo.create("tr", {innerHTML: r}, table, 'last');
      
      var v = new dijit.form.ValidationTextBox({
         // id: "varcharwidthid",
         regExp: "\\d+",
         promtMessage: "Enter varchar field with for Object Attribute Type",
         invalidMessage: "Invalid Object Attribute Varchar width (must be a decimal number)",
         trim: true,
         value: "xxxx",
         required: true });
         dojo.place(v, val2, "last");
   } else {
      var r = '<th>Enum values</th><td><input type="text" name="enumvalues" value="' + val + '"/></td>';
      dojo.create("tr", {innerHTML: r}, table, 'last');
   }

}


function showVarcharOrEnum(typeSelectorId) {

   var typeSelector = dojo.byId(typeSelectorId);
   var selectedType = typeSelector.options[typeSelector.selectedIndex].value;   /* will be 'varchar' or 'enum' */
   var table = dojo.byId("createobjectattributetableid");
   var val1 = dojo.byId("val1");
   var inp1 = dijit.byId("inp1");
   var td1 =  dojo.byId("td1");
  
   
   if (selectedType == 'varchar') {
      val1.innerHTML = 'Varchar Width';
      if (inp1 != null) {
         inp1.destroyRecursive();
         dojo.create("input", {id:"inp1", type:"text", name:"varcharwidth", innerHTML: ""}, td1);
      };
 //     var v = new dijit.form.ValidationTextBox({
 //        id: "inp1",
 //        regExp: "\\d+",
 //        promtMessage: "Enter varchar field with for Object Attribute Type",
 //        invalidMessage: "Invalid Object Attribute Varchar width (must be a decimal number)",
 //        trim: true,
 //        required: true }, "inp1");
 //     dojo.place(v, inp1, "last");
   } else {
 
      val1.innerHTML = 'Enum Values';
      if (inp1 != null) {
         inp1.destroyRecursive();
         dojo.create("input", {id:"inp1", type:"text", name:"enumvalues", innerHTML: ""}, td1);
      };
 //     var v = new dijit.form.ValidationTextBox({
 //        id: "inp1",
 //        regExp: ".*",
 //        promtMessage: "Enter a comma separated list of Object Attribute Values, enclosed in single quotes, eg \'blue\',\'green\',\'red\'",
 //        invalidMessage: "Invalid Object Attribute Varchar width (must be 1 to ? characters)",
 //        trim: true,
 //        required: true }, "inp1");
 //     dojo.place(v, inp1, "last");
   }

}






	

