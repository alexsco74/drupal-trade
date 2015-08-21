(function ($) {

    Drupal.behaviors.pik = {
        attach: function (context, settings) {

            //image loading process
            function pikImgLoadInit() {
                $('img.trade-img-data-src[data-src]').each(
                    function (i) {
                        $(this).attr('src', $(this).attr('data-src'));
                        $(this).removeAttr('data-src');
                    });
            }

            $('.l-region--navigation .block--nav-bar', context).once().each(function () {
                var thisMenu = $(this);

                function pikSetVerticalAlignMiddle(thisLinkLocal) {
                    var paddingTopLink = parseInt((thisLinkLocal.parent().outerHeight() - thisLinkLocal.outerHeight()) / 2, 10);
                    thisLinkLocal.parent().css({
                        'padding-top': paddingTopLink + 'px'
                    });
                }

                function pikProcessMainMenu(thisMenuLocal) {
                    $('li a', thisMenuLocal).each(function () {
                        pikSetVerticalAlignMiddle($(this));
                    });
                }

                pikProcessMainMenu(thisMenu);

                $(window).resize(function () {
                    pikProcessMainMenu(thisMenu);
                });
            });

            // region animation
            $('.pik-animation-region-link', context).once().each(function () {
                var animationLink = $(this);
                var animationContainer = $('.pik-animation-region-content', animationLink.parent());
                animationLink.live('click', function (e) {
                    e.preventDefault();
                    if (animationContainer.hasClass('pik-is-open')) {
                        animationContainer.removeClass('pik-is-open');
                        animationLink.removeClass('pik-link-is-active');
                    } else {
                        animationContainer.addClass('pik-is-open');
                        animationLink.addClass('pik-link-is-active');
                    }
                });
            });

            $(window).load(
                function () {
                    pikImgLoadInit();
                }
            );

            $(document).bind('documentChange', function () {
                pikImgLoadInit();
            });

        }
    };

})(jQuery);
