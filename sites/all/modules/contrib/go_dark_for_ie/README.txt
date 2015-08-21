CONTENTS OF THIS FILE
---------------------

 * Summary
 * Requirements
 * Installation
 * Configuration


SUMMARY
-------

Current Maintainer: James An <james at jamesan dot ca>

This module helps websites "Go Dark For IE". The Go Dark For IE movement
proposes that on October 26, 2012 websites blackout for all users visiting your
website using IE8 or older and instead direct them to Go Dark For IE with
information about the movement and upgrading or switching their browser."
October 26, 2012 is the general availability date for Windows 8, which will be
packaged with IE10.

You can add your website to the campaign by submitting its URL to:
  http://godarkforie.org/#extra.

I am in no way affiliated with the Go Dark For IE folks. Actually, I don't even
know who's running it and nor does anyone apparently: 
  http://www.neowin.net/news/go-dark-for-ie-effort-launched-for-better-web-browser-use

For a full description of the module, visit the project page:
  http://drupal.org/project/go_dark_for_ie

To submit bug reports and feature suggestions, or to track changes:
  http://drupal.org/project/issues/go_dark_for_ie

REQUIREMENTS
------------

Drupal 7.

INSTALLATION
------------

Install as usual. See http://drupal.org/documentation/install/modules-themes/modules-7
for more information on installing Drupal modules.

CONFIGURATION
-------------

The blackout screen is enabled by default (i.e. it will blackout your website on
October 26, 2012 for all users visiting your website using IE8 or older) when
the module is enabled.

Disable or preview the blackout in:
  Administration » Configuration » User interface » Go Dark For IE.

Configure user permissions in:
  Administration » People » Permissions

The "Ignore Go Dark For IE screen" permission prevents the blackout screen to
appear even if the user is using IE8 or older on October 26, 2012.
