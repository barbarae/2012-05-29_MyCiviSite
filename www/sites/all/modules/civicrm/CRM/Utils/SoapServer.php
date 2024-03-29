<?php

/*
 +--------------------------------------------------------------------+
 | CiviCRM version 4.1                                                |
 +--------------------------------------------------------------------+
 | Copyright CiviCRM LLC (c) 2004-2011                                |
 +--------------------------------------------------------------------+
 | This file is a part of CiviCRM.                                    |
 |                                                                    |
 | CiviCRM is free software; you can copy, modify, and distribute it  |
 | under the terms of the GNU Affero General Public License           |
 | Version 3, 19 November 2007 and the CiviCRM Licensing Exception.   |
 |                                                                    |
 | CiviCRM is distributed in the hope that it will be useful, but     |
 | WITHOUT ANY WARRANTY; without even the implied warranty of         |
 | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.               |
 | See the GNU Affero General Public License for more details.        |
 |                                                                    |
 | You should have received a copy of the GNU Affero General Public   |
 | License and the CiviCRM Licensing Exception along                  |
 | with this program; if not, contact CiviCRM LLC                     |
 | at info[AT]civicrm[DOT]org. If you have questions about the        |
 | GNU Affero General Public License or the licensing of CiviCRM,     |
 | see the CiviCRM license FAQ at http://civicrm.org/licensing        |
 +--------------------------------------------------------------------+
*/

/**
 *
 * @package CRM
 * @copyright CiviCRM LLC (c) 2004-2011
 * $Id$
 *
 */


/**
 * This class handles all SOAP client requests.
 *
 * @package CRM
 * @copyright CiviCRM LLC (c) 2004-2011
 * $Id$
 *
 */

class CRM_Utils_SoapServer
{
    /**
     * Number of seconds we should let a soap process idle
     * @static
     */
    static $soap_timeout = 0;
    
    /**
     * Cache the actual UF Class
     */
    public $ufClass;

    /**
     * Class constructor.  This caches the real user framework class locally,
     * so we can use it for authentication and validation.
     *
     * @param  string $uf       The userframework class
     */
    public function __construct() {
        // any external program which call SoapServer is responsible for
        // creating and attaching the session
        $args = func_get_args( );
        $this->ufClass = array_shift( $args );
    }

    /**
     * Simple ping function to test for liveness.
     *
     * @param string $var   The string to be echoed
     * @return string       $var
     * @access public
     */
    public function ping($var) {
        $session = CRM_Core_Session::singleton();
        $key = $session->get('key');
        $session->set( 'key', $var );
        return "PONG: $var ($key)";
    }


    /**
     * Verify a SOAP key
     *
     * @param string $key   The soap key generated by authenticate()
     * @return none
     * @access public
     */
    public function verify($key) {
        $session = CRM_Core_Session::singleton();

        $soap_key = $session->get('soap_key');
        $t = time();
        
        if ( $key !== sha1($soap_key) ) {
            throw new SoapFault('Client', 'Invalid key');
        }
        

        if (    self::$soap_timeout && 
                $t > ($session->get('soap_time') + self::$soap_timeout)) {
            throw new SoapFault('Client', 'Expired key');
        }
        
        /* otherwise, we're ok.  update the timestamp */
        $session->set('soap_time', $t);
    }
    
    /**
     * Authentication wrapper to the UF Class
     *
     * @param string $name      Login name
     * @param string $pass      Password
     * @return string           The SOAP Client key
     * @access public
     * @static
     */
    public function authenticate($name, $pass, $loadCMSBootstrap = false ) {
        require_once( str_replace( '_', DIRECTORY_SEPARATOR, $this->ufClass ) . '.php' );
        eval ('$result =& ' . $this->ufClass . '::authenticate($name, $pass, $loadCMSBootstrap );');

        if (empty($result)) {
            throw new SoapFault('Client', 'Invalid login');
        }
        
        $session = CRM_Core_Session::singleton();
        $session->set('soap_key', $result[2]);
        $session->set('soap_time', time());
        
        return sha1($result[2]);
    }

    /*** MAILER API ***/
    public function mailer_event_bounce($key, $job, $queue, $hash, $body) {
        $this->verify($key);
        $params = array( 'job_id'         => $job,
                         'time_stamp' => date('YmdHis'),
                         'event_queue_id' => $queue,
                         'hash'           => $hash,
                         'body'           => $body,
                         'version'        => 3
                         );
        return civicrm_api('Mailing', 'event_bounce', $params);
    }
    
    public function mailer_event_unsubscribe($key, $job, $queue, $hash) {
        $this->verify($key);
        $params = array( 'job_id'         => $job,
                         'time_stamp' => date('YmdHis'),
                         'org_unsubscribe' => 0,
                         'event_queue_id' => $queue,
                         'hash'           => $hash,
                         'version'        => 3
                         );
        return civicrm_api('MailingGroup', 'event_unsubscribe', $params);
    }

    public function mailer_event_domain_unsubscribe($key, $job, $queue, $hash) {
        $this->verify($key);
        $params = array( 'job_id'         => $job,
                         'time_stamp' => date('YmdHis'),
                         'org_unsubscribe' => 1,
                         'event_queue_id' => $queue,
                         'hash'           => $hash,
                         'version'        => 3
                         );
        return civicrm_api('MailingGroup', 'event_domain_unsubscribe', $params);
    }

    public function mailer_event_resubscribe($key, $job, $queue, $hash) {
        $this->verify($key);
        $params = array( 'job_id'         => $job,
                         'time_stamp' => date('YmdHis'),
                         'org_unsubscribe' => 0,
                         'event_queue_id' => $queue,
                         'hash'           => $hash,
                         'version'        => 3
                         );
        return civicrm_api('MailingGroup', 'event_resubscribe', $params);
    }

    public function mailer_event_subscribe($key, $email, $domain, $group) {
        $this->verify($key);
        $params = array( 'email'          => $email,
                         'group_id'       => $group,
                         'version'        => 3
                         );
        return civicrm_api('MailingGroup', 'event_subscribe', $params);
    }
    
    public function mailer_event_confirm($key, $contact, $subscribe, $hash) {
        $this->verify($key);
        $params = array( 'contact_id'   => $contact,
                         'subscribe_id' => $subscribe,
                         'time_stamp' => date('YmdHis'),
                         'event_subscribe_id' => $subscribe,
                         'hash'         => $hash,
                         'version'      => 3
                         );
        return civicrm_api('Mailing', 'event_confirm', $params);
    }
    
    public function mailer_event_reply($key, $job, $queue, $hash, $bodyTxt, $rt, $bodyHTML = null, $fullEmail = null) {
        $this->verify($key);
        $params = array( 'job_id'         => $job,
                         'event_queue_id' => $queue,
                         'hash'           => $hash,
                         'bodyTxt'        => $bodyTxt,
                         'replyTo'        => $rt,
                         'bodyHTML'       => $bodyHTML,
                         'fullEmail'      => $fullEmail,
                         'time_stamp' => date('YmdHis'),
                         'version'        => 3
                         );
        return civicrm_api('Mailing', 'event_reply', $params);
    }
    
    public function mailer_event_forward($key, $job, $queue, $hash, $email) {
        $this->verify($key);
        $params = array( 'job_id'         => $job,
                         'event_queue_id' => $queue,
                         'hash'           => $hash,
                         'email'          => $email,
                         'version'        => 3
                         );
        return civicrm_api('Mailing', 'event_forward', $params);
    }
    
    public function get_contact($key, $params) { 
        $this->verify($key);
        $params['version'] = 3;
        return civicrm_api( 'contact', 'get', $params );
    }

}


