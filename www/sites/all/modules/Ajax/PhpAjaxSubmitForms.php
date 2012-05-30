<?php
class PhpAjaxSubmitForms {
	private $url;
	private $method;
	private $queryString;
	private $params = array();
	
	public function PhpAjaxSubmitForms ($url = "default.php", $method = "GET", $queryString = "sid=Math.Random()") {
		if(preg_match ("/(.php|.asp|.aspx|.jsp)$/", $url)){
			$this->url = $url;
			$this->queryString = $queryString;
			if($method == "GET" || $method == "POST") {
				$this->method = $method;	
			} else {
				throw new Exception("That method doesn't exist.");
			}
		} else {
			throw new Exception("Url of file is not valid");
		}
	}
	
	public function CreateObjectFunction () {
		print "var xmlHttp = false;\n";
		print "function CreateObject() {\n";
		print "\tif(window.ActiveXObject) {\n";
		print "\ttry {\n";
		print "\t\treturn new ActiveXObject(\"Msxml2.XMLHTTP\")\n";
		print "\t} catch (e) {\n";
		print "\t\ttry {\n";
		print "\t\t\treturn new ActiveXObject(\"Microsoft.XMLHTTP\");\n";
		print "\t\t} catch (E) {\n";
		print "\t\t\treturn false;\n";
		print "\t\t}\n";
		print "\t}\n";
		print "\t}\n";
		print "\tif(window.XMLHttpRequest) {\n";
		print "\t\ttry {\n";
		print "\t\t\treturn new XMLHttpRequest();\n";
		print "\t\t} catch (e) {\n";
		print "\t\t\treturn false;\n";
		print "\t\t}\n";
		print "\t}\n";
		print "\tif(window.createRequest()) {\n";
		print "\t\ttry {\n";
		print "\t\t\treturn new window.createRequest();\n";
		print "\t\t} catch (e) {\n";
		print "\t\t\treturn false;\n";
		print "\t\t}\n";
		print "\t}\n";
		print "}\n";
	}
	
	public function SubmitFormFunction ($params = array(), $responseID) {
		$this->params = $params;
		if(empty($this->params)) {
			throw new Exception("Expecting some parameters of the form data.");
		}
		$ReqFields = array();
		print "function SubmitForm () {\n";
		foreach ($this->params as $key => $value) {
			$filtered = str_replace("*", "", $value);
			print "\tvar ".$filtered." = document.getElementById('".$filtered."').value;\n";
			if(preg_match('/\*/', $value)) {
				array_push($ReqFields, $filtered." == '' ") ;	
			}
		}
		print "\tif( " . implode("|| ", $ReqFields) . ") {\n";
		print "\t\t" . $this->UserErrorFunction() . "\n";
		print "\t\treturn false;\n";
		print "\t}\n";
		print "\tSendRequest();\n";
		print "\treturn false;\n";
		print "}\n";
		$this->CreateObjectFunction();
		$this->RequestResultFunction($responseID);
		$this->SendRequestFunction();
	}
	
	
	
	public function UserErrorFunction () {
		return "alert('You didn\'t fill all the fields.');";
	}
	
	
	public function SendRequestFunction () {
		if($this->url == "" || $this->queryString == "") {
			throw new Exception ("You cannot send request some parametars are missing.");
		}
		print "function SendRequest () {\n";
		print "\txmlHttp = CreateObject ();\n";
		print "\tif(xmlHttp == false) {\n";
		print "\t\talert('Error creating XHR object');\n";
		print "\t\treturn;\n";
		print "\t}\n";
		if(!empty($this->params)) {
			$Fields = array ();
			foreach ($this->params as $key => $value) {
				$filtered = str_replace("*", "", $value);
				print "\tvar ".$filtered." = document.getElementById('".$filtered."');\n";
				array_push($Fields,$filtered."='+".$filtered.".value+'");	
			}
			$this->queryString = implode("&", $Fields);
		}
		print "\txmlHttp.open(\"".$this->method."\", '".$this->url."?".$this->queryString."', true);\n";
		print "\t\tdocument.getElementById('loader').style.display = 'block';\n";
		print "\txmlHttp.onreadystatechange = requestResult;\n";
		print "\txmlHttp.send();\n";
		print "}\n";
	}
	
	public function RequestResultFunction ($div_id) {
		print "function requestResult () {\n";
		print "\tif( xmlHttp.readyState == 4 ) {\n";
		print "\t\tdocument.getElementById('".$div_id."').innerHTML = xmlHttp.responseText;\n";
		print "\t\tdocument.getElementById('loader').style.display = 'none';\n";
		print "\t}\n";	
		print "}\n";	
	}
}
?>