<?php

/**
 * @file
 * Handler for 'content_simple' display.
 */
class mediareference_plugin_display_noexposed extends views_plugin_display {
  function execute() {
    return $this->view->render($this->display->id);
  }

  function render() {
    return !empty($this->view->result) || !empty($this->view->style_plugin->definition['even empty']) ? $this->view->style_plugin->render($this->view->result) : '';
  }

  function displays_exposed() {
    return FALSE;
  }
}