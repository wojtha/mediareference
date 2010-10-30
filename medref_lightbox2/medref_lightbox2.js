// $Id: $

/**
 * Stripped down method Lightbox.start to allow us launch only "Lightframe" and also do it programatically
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

Drupal.settings.lightframe = Drupal.settings.lightframe || {};

Drupal.settings.lightframe.start = function(href) {
  var alt = title = 'Media Browser';
  var style = 'width:900px; height:500px; scrolling:yes; disableCloseClick:false';
  LightframeStart(href, title, alt, style);
};