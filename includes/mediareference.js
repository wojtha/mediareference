// $Id: $

/**
 * Custom event listeners order management function
 *
 * Let us to place event listener to the first place
 *
 * See:
 * - http://snipplr.com/view/13556/jquery-event-stack-binder/
 * - http://snipplr.com/view/13515/jstack--jquery-event-stack-management/
 */
(function($) {
  $.fn.bindIntoStack = function(pos, namespace, func) {
    var evt = namespace.split('.').shift();

    return this.each(function() {
      var $this = $(this);

      $this.bind(namespace, func);

      var newOrder = new Array();

      $.each($this.data('events')[evt], function(k) {
        newOrder.push(k);
      });

      if (newOrder.length > pos + 1) {
        newOrder.splice(pos, 0, newOrder.pop());

        var evts = new Object();

        $.each(newOrder, function(k) {
          evts[this] = $this.data('events')[evt][this];
        });

        $this.data('events')[evt] = evts;
      }
    });
  };
})(jQuery);




/**
 *  Trigger update event on the button which started the lightframe
 */
function LightframeUpdate() {
  if (Drupal.settings.lightframe.updateAfterClose) {
    $(Drupal.settings.lightframe.updateAfterClose).trigger('update');
    Drupal.settings.lightframe.updateAfterClose = null;
  }
}

// Make sure that namespace exists
Drupal.settings.mediareference = Drupal.settings.mediareference || {};
Drupal.settings.lightframe = Drupal.settings.lightframe || {};

//==========================================
// DRUPAL BEHAVIOURS
//==========================================

Drupal.behaviors.mediareference = function(context) {

  $('.mrf-browse-button:not(.mrf-processed)', context)
    .addClass('mrf-processed')
    .each( function() {
      // our custom property - a kind of semaphore which disable listeners launch during mousedown event
      this.mouseDownStopPropagation = 1;
    })
    // we are using custom jQuery function to attach our handler to the first place in the event handlers queue
    .bindIntoStack(0, 'mousedown.override', function(e) {
      // we stop event propagation and bubbling because there is a AHAH binded to this event
      // Other handlers in queue (including AHAH, can be launched only when setting this.mouseDownStopPropagation to 0 before
      if (this.mouseDownStopPropagation) {
        e.stopImmediatePropagation();
        // parse type and field arguments from AHAH settings
        // TODO: think about some better solution how to get these values - this is little bit dirty
        var ahahSettings = Drupal.settings.ahah[$(this).attr('id')];
        if (ahahSettings) {
          var params = ahahSettings.url.split('/');
          var href = '/mediabrowser/' + params[2] + '/' + params[3];
          // launch browser in popup
          Drupal.settings.lightframe.start(href);
          // save this to global variable for later use
          Drupal.settings.lightframe.updateAfterClose = this;
        }
      }
      // Prevent default action of the link click event.
      return false;
    })
    // our custom event which fires all attached mousedown listeners
    .bind('update', function() {
      this.mouseDownStopPropagation = 0;
      $(this).trigger('mousedown');
      this.mouseDownStopPropagation = 1;
    });

  // In lightframe add "&lightframe=1" to all links on the page
  if (Drupal.settings.mediareference.lightframize) {
    $('a:not(.lightframized)', context).each( function() {
      var link = $(this).attr('href');
      if (link.search(/lightframe=/) == -1) {
        if (link.search(/\?/) == -1) {
          $(this).attr('href', link + '?lightframe=1');
        }
        else {
          $(this).attr('href', link + '&lightframe=1');
        }
      }
    })
    .addClass('lightframized');
  }
};