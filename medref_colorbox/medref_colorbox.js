// $Id: $

Drupal.settings.lightframe = Drupal.settings.lightframe || {};

Drupal.settings.lightframe.start = function(href) {
  var alt = title = 'Media Browser';
  $.colorbox({href: href, overlayClose: false, width:'900px', height:'600px', iframe: true});
};