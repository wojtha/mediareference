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


/**
 * Stripped down method Lightbox.start to allow us launch only "Lightframe" programatically
 */
function LightframeStart(href, title, alt, style) {

  Lightbox.isPaused = !Lightbox.autoStart;

  // Replaces hideSelectBoxes() and hideFlash() calls in original lightbox2.
  Lightbox.toggleSelectsFlash('hide');

  Lightbox.isSlideshow = false;
  Lightbox.isLightframe = true;
  Lightbox.isVideo = false;
  Lightbox.isModal = false;
  Lightbox.imageArray = [];
  Lightbox.imageNum = 0;

  // Stretch overlay to fill page and fade in.
  var arrayPageSize = Lightbox.getPageSize();
  $("#lightbox2-overlay")
    .hide()
    .css({
      'width': '100%',
      'zIndex': '10090',
      'height': arrayPageSize[1] + 'px',
      'backgroundColor' : '#' + Lightbox.overlayColor,
      'opacity' : Lightbox.overlayOpacity
    })
    .removeClass("overlay_macff2")
    .addClass("overlay_default")
    .fadeIn(Lightbox.fadeInSpeed);

  Lightbox.imageArray.push([href, title, alt, style]);

  $('#frameContainer, #modalContainer, #lightboxImage').hide();
  $('#hoverNav, #prevLink, #nextLink, #frameHoverNav, #framePrevLink, #frameNextLink').hide();
  $('#imageDataContainer, #numberDisplay, #bottomNavZoom, #bottomNavZoomOut').hide();
  $('#outerImageContainer').css({'width': '250px', 'height': '250px'});

  // Calculate top and left offset for the lightbox.
  var arrayPageScroll = Lightbox.getPageScroll();
  var lightboxTop = arrayPageScroll[1] + (Lightbox.topPosition == '' ? (arrayPageSize[3] / 10) : Lightbox.topPosition) * 1;
  var lightboxLeft = arrayPageScroll[0];
  $('#lightbox')
    .css({
      'zIndex': '10500',
      'top': lightboxTop + 'px',
      'left': lightboxLeft + 'px'
    })
    .show();

  Lightbox.total = Lightbox.imageArray.length;
  Lightbox.changeData(Lightbox.imageNum);
};

function LightframeUpdate() {
  if (Drupal.settings.mediareference.lightframe.updateAfterClose) {
    $(Drupal.settings.mediareference.lightframe.updateAfterClose).trigger('update');
    Drupal.settings.mediareference.lightframe.updateAfterClose = null;
  }
}

// Make sure that namespace exists
Drupal.settings.mediareference = Drupal.settings.mediareference || {};
Drupal.settings.mediareference.lightframe = Drupal.settings.mediareference.lightframe || {};

// =========================================
// DRUPAL BEHAVIOURS
// =========================================

Drupal.behaviors.mediareference = function(context) {

  $('.mediareference-browse-button', context)
    .bindIntoStack(0, 'mousedown.override', function(e) {
      if (this.mouseDownStopPropagation) {
        e.stopImmediatePropagation();
        var href = '/mediabrowser';
        var alt = title = 'Media Browser';
        var style = 'width:900px; height:500px; scrolling:yes; disableCloseClick:false';
        LightframeStart(href, title, alt, style);
        Drupal.settings.mediareference.lightframe.updateAfterClose = this;
      }
      // Prevent default action of the link click event.
      return false;
    })
    .bind('update', function() {
      this.mouseDownStopPropagation = 0;
      $(this).trigger('mousedown');
      this.mouseDownStopPropagation = 1;
    })
    .each( function() {
      this.mouseDownStopPropagation = 1;
    });

};