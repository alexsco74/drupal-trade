(function ($) {

    Drupal.behaviors.trade = {
        attach: function (context, settings) {
            var waitTag = $('<div class="trade-ajax-load-overlay"></div><span class="trade-ajax-load-process-wait"></span>');

            // load content ajax
            function tradeLoadContentInit() {
                $('a[data-ajax-sel]').once().each(function () {
                    var thisLink = $(this);
                    thisLink.bind('click', function () {
                        var href = thisLink.attr('href');
                        var thisSel = thisLink.attr('data-ajax-sel');
                        var cloneWait = waitTag.clone();
                        $(thisSel).addClass('trade-ajax-load-process');
                        // $(thisSel).wrapInner('<div class="trade-ajax-load-process-inner"></div>');
                        $(thisSel).prepend(cloneWait);
                        $(thisSel).load(href + ' ' + thisSel + ' > *', function () {
                            $(document).trigger('documentChange');
                            cloneWait.remove();
                            $(thisSel).removeClass('trade-ajax-load-process');
                        });
                        return false;
                    });
                });

                // checkbox change load content ajax
                $('input:checkbox[data-ajax-sel]').once().each(function () {
                    var thisLink = $(this);
                    thisLink.bind('change', function () {
                        var href = thisLink.attr('data-ajax-href');
                        var thisSel = thisLink.attr('data-ajax-sel');
                        var cloneWait = waitTag.clone();
                        $(thisSel).addClass('trade-ajax-load-process');
                        // $(thisSel).wrapInner('<div class="trade-ajax-load-process-inner"></div>');
                        $(thisSel).prepend(cloneWait);
                        $(thisSel).load(href + ' ' + thisSel + ' > *', function () {
                            $(document).trigger('documentChange');
                            cloneWait.remove();
                            $(thisSel).removeClass('trade-ajax-load-process');
                        });
                    });
                });

            };

            $(window).load(function () {
                $('.trade-opacity').addClass('trade-opacity-slide-show');
            });

            tradeLoadContentInit();

            $(document).bind('documentChange', function () {
                tradeLoadContentInit();
            });
        }
    };
})(jQuery);