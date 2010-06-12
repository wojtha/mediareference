// $Id: $

// Event order management
// http://snipplr.com/view/13556/jquery-event-stack-binder/
// http://snipplr.com/view/13515/jstack--jquery-event-stack-management/

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

// =========================================
// DRUPAL BEHAVIOURS
// =========================================

Drupal.behaviors.mediareference = function(context) {

  //$('div.view-mediabrowser a').addClass('ctools-use-dialog');


  // This is our onSubmit callback that will be called from the child window
  // when it is requested by a call to modalframe_close_dialog() performed
  // from server-side submit handlers.
  function onSubmitCallback(args) {
    alert('Finish!');
  }

  $('#edit-field-slideshow-0-actions-browse', context).bindIntoStack(0, 'mousedown.override', function(e) {
  //    var id = $(this).attr('rel');
  //    var field_name = $('#' + id).attr('rel');

   // Build modal frame options.
    var modalOptions = {
      onSubmit: onSubmitCallback,
      url: Drupal.settings.basePath + 'mediabrowser',
      width: 950,
      height: 600,
      autoFit: false
    };

    // Open the modal frame dialog.
    Drupal.modalFrame.open(modalOptions);

  //    e.stopImmediatePropagation();

  // Prevent default action of the link click event.
  return false;
  });


};