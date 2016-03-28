
function selectDuration(durationSelector, phaseSelectorId) {

  var selectedDurationType = durationSelector.options[durationSelector.selectedIndex].value;
  
  if ((selectedDurationType == 'RunforNminutes') || (selectedDurationType == 'TimeoutafterNminutes')) {
  
     //document.getElementById("maxDurationID").innerHTML = '<input id="maxDurationID" type="text" name="maxduration" size=10 maxlength=10 />';	            

     var myNewValidationBox = new dijit.form.ValidationTextBox({ 
                                       id : "validationBox",
                                       selectOnClick : true,
                                       name : "validationBox",
                                       trim : true, 
                                       regExp : "\\d{1,8}",
                                       required : false,
                                       promptMessage : "Enter the maximum duration for the testcase in seconds",
                                       invalidMessage : "Max Duration must be 0-999999999)"
                                      }); 
            //add to view 
       dojo.byId("maxDurationID").appendChild(myNewValidationBox.domNode);

   } else {
       var validationdijit = dijit.byId("validationBox");
       if (validationdijit != null) {
         validationdijit.destroyRecursive();
       };
   }
}


function selectHostSelectorValues(hostSelector, hostSelectortdId) {

      var hostSelectorType = hostSelector.options[hostSelector.selectedIndex].value;
      
      if (hostSelectorType == 'BySelectorValue') {

         // Issue an asynchronous request to the server for a list of all host selector values
         // json data will be returned in the form:
         // [ { ID: "11", Value: "AIX"}, ... ]
         dojo.xhrGet({
	    	url: "rest/daf/hostselectorvalue/",
		    load: function(data, ioargs) { 
		    
		         var sel = '';
		         var sid = dojo.byId(hostSelectortdId);
		         dojo.create("select", {innerHTML: "", id: "hostselectorvalueId", name: "hostselectorvalue"}, sid);
		         for (var i=0; i<data.length; i++) {
		            dojo.create("option", {innerHTML: data[i]["Value"], value: data[i]["Value"]}, "hostselectorvalueId");
		         }
		      },
	    	error: function(data, ioargs) {
		         alert("An error occurred while invoking selectHostSelectorValues()");     /* <<<<< need a better error handler */
		      },
		   handleAs: 'json'
	     });
	     	  
	  } else {
	  
         dojo.empty(hostSelectortdId);	  
	  }
}


function selectHostSelectorValues_save(hostSelector, hostSelectorId) {

      var hostSelectorType = hostSelector.options[hostSelector.selectedIndex].value;
      
      if (hostSelectorType == 'BySelectorValue') {

         // Issue an asynchronous request to the server for a list of all host selector values
         // json data will be returned in the form:
         // [ { ID: "11", Value: "AIX"}, ... ]
         dojo.xhrGet({
	    	url: "rest/daf/hostselectorvalue/",
		    load: function(data, ioargs) {
		         var sel = '<select id="hostselectorvalueId" name="hostselectorvalue">';
		         for (var i=0; i<data.length; i++) {
		            sel += '<option value="' + data[i]["Value"] + '">' + data[i]["Value"] + '</option>';
		         }
		         sel += '</select>';
		         document.getElementById(hostSelectorId).innerHTML = sel;	            
		      },
	    	error: function(data, ioargs) {
		         alert("An error occurred while invoking selectHostSelectorValues()");     /* <<<<< need a better error handler */
		      },
		   handleAs: 'json'
	     });
	     	  
	  } else {
	  
	     document.getElementById(hostSelectorId).innerHTML = "";	            
	  
	  }
}



