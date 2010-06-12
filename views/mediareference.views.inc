<?php

/**
 * Implementation of hook_views_plugins.
 *
 * Defines some plugins used by the Views modes for
 * nodereference and userreference.
 */
function mediareference_views_plugins() {
  $plugins = array(
    'module' => 'mediareference', // This just tells our themes are elsewhere.
    'display' => array(
      'mediareference_noexposed' => array(
        'title' => 'View without exposed filters',
        'help' => 'Returns the rendered view without exposed filter',
        'handler' => 'mediareference_plugin_display_noexposed',
        'path' => drupal_get_path('module', 'mediareference') . '/views',
        'no ui' => TRUE, // Programmatic use only.
        'uses hook menu' => FALSE,
        'use ajax' => FALSE,
        'use pager' => FALSE,
        'accept attachments' => FALSE,
      ),
    ),
  );
  return $plugins;
}