(function ($) {

    Drupal.behaviors.tradeUpholsteryPicker = {
        attach: function (context, settings) {

            // upholstery load materials
            var waitTag = $('<div class="trade-ajax-load-overlay trade-ajax-load-overlay-center"></div><span class="trade-ajax-load-process-wait trade-ajax-load-process-wait-center"></span>');

            function tradeUpholsteryPickerLoadMaterials(thisContIn, nidIn, parentIn, linkLoader) {
                var thisContainer = thisContIn;
                var dataNid = $(this).attr('data-nid');
                var link = '/trade/product/' + nidIn + '/upholstery/materials/' + parentIn;

                // thisContainer.addClass('trade-ajax-load-process');
                // $('li:first', thisContainer).append(waitTag);
                var cloneWait = waitTag.clone();
                thisContainer.addClass('trade-ajax-load-process');
                thisContainer.prepend(cloneWait);

                $.ajax({
                    type: 'POST',
                    data: {
                        trade_nid: nidIn
                    },
                    url: link,
                    success: function (response) {

                        if (response.status) {
                            if (response.data) {
                                linkLoader.removeClass('trade-ajax-content-need-load');
                                thisContainer.replaceWith(response.data);
                                $(document).trigger('documentChange');
                                // $('.trade-order-container').fadeIn();
                            }
                            thisContainer.removeClass('trade-ajax-load-process');
                        }

                        // open all
                        cloneWait.remove();
                        thisContainer.removeClass('trade-ajax-load-process');
                        $('.trade-ajax-load-process-wait', thisContainer).remove();
                        linkLoader.removeClass('trade-ajax-content-load-process');
                    },
                    error: function (xmlhttp) {
                        cloneWait.remove();
                        thisContainer.removeClass('trade-ajax-load-process');
                        $('.trade-ajax-load-process-wait', thisContainer).remove();
                        linkLoader.removeClass('trade-ajax-content-load-process');
                        // alert(Drupal.ajaxError(xmlhttp, link));
                    }
                });
            };


            function tradeUpholsteryPickerInit() {
                var overlay = $('<div class="trade-vertical-overlay"></div>');

                overlay.click(function () {
                    $(this).hide();
                });


                // set ma width
                $(window).bind('resize', function () {
                    var upContWidth = $(window).width();
                    var upContSetWidth = upContWidth * 0.7;
                    $('.trade-vertical-tabs-active.trade-product-option-upholstery-picker-container .trade-vertical-tabs-horizontal-side-inner').css({'max-width': upContSetWidth + 'px'});
                });

                function tradeUpholsteryPickerSetActive(thisElIn) {
                    var htmlHeight = $('html').outerHeight();
                    $('body').prepend(overlay.css({'height': htmlHeight + 'px'}).show());
                    // active vertical tabs
                    thisElIn.addClass('trade-vertical-tabs-active');
                    // alert('Active');
                };

                function tradeUpholsteryPickerSetInactive(thisElIn) {
                    overlay.hide();
                    thisElIn.removeClass('trade-vertical-tabs-active');

                    // hide ul collection & material
                    $('ul[data-level]:not(.trade-state-close)', thisElIn).addClass('trade-state-close');

                    // init Uho
                    tradeUpholsteryPickerInitUho(thisElIn);

                    // nav link remove active state
                    $('.trade-nav-link[data-level="1"]').removeClass('trade-state-active');
                };

                function tradeUpholsteryPickerInitUho(thisElIn) {
                    var thisFirstCellInnerHeight = $('.trade-vertical-tabs-cell-inner:first', $('.trade-vertical-tabs-vertical-side-inner-uho', thisElIn).parent()).outerHeight();
                    var thisHorizontalTabHeight = $('.trade-vertical-tabs-horizontal-side', thisElIn).outerHeight();
                    var thisHorizontalTabWidth = $('.trade-vertical-tabs-horizontal-side', thisElIn).outerWidth();

                    // alert(thisHorizontalTabHeight + ' ' + thisHorizontalTabWidth);

                    var thisUhoHeight = (thisHorizontalTabHeight - thisFirstCellInnerHeight);

                    // alert(thisFirstCellInnerHeight + ' ' + thisCellHeight);

                    if (thisUhoHeight !== undefined) {

                        // alert(thisUhoHeight);
                        if (thisUhoHeight < 0 || thisHorizontalTabWidth == 0 || !thisElIn.hasClass('trade-vertical-tabs-active')) {
                            thisUhoHeight = 0;
                        }

                        $('.trade-vertical-tabs-vertical-side-inner-uho', thisElIn).css({'height': thisUhoHeight + 'px'});
                    }
                };

                // relation price with use
                $('.trade-product-option-upholstery-picker-container').once().each(function () {
                    var thisEl = $(this);
                    var thisNid = $(this).attr('data-nid');
                    // tooltip
                    thisEl.tooltip({
                        items: ".trade-thumb-wr",
                        tooltipClass: 'trade-product-color-picker',
                        content: function () {
                            var thisWr = $(this).parent();
                            if (typeof $('.trade-tooltip-content', thisWr) != undefined) {
                                return $('.trade-tooltip-content', thisWr).html();
                            }
                        }
                    });

                    // close overlay on event
                    $('.trade-vertical-tabs-vertical-side-inner-uho', thisEl).live('click', function () {
                        tradeUpholsteryPickerSetInactive(thisEl);
                    });

                    // close overlay on click on
                    overlay.bind('click', function () {
                        tradeUpholsteryPickerSetInactive(thisEl);
                        // $(this).hide();
                    });

                    //close overlay on Esc
                    $(window).keyup(function (event) {
                        if (event.which == 27) {
                            tradeUpholsteryPickerSetInactive(thisEl);
                        }
                    });

                    //nav
                    $('.trade-nav-link', thisEl).live('click', function () {

                        if ($(this).is(':not(.trade-state-active)')) {
                            var thisLevel = $(this).attr('data-level');
                            var thisId = $(this).attr('data-id');


                            //overlay
                            if (thisLevel == 1 && !thisEl.hasClass('trade-vertical-tabs-active')) {
                                tradeUpholsteryPickerSetActive(thisEl);
                            }

                            $('.trade-nav-link[data-level="' + thisLevel + '"]', thisEl).removeClass('trade-state-active');

                            if (thisLevel == 1) {

                                $('ul[data-level="2"]', thisEl).each(function () {
                                    $(this).addClass('trade-state-close');
                                });

                                if ($('.trade-state-active[data-level="2"]', $('ul[data-parent="' + thisId + '"]', thisEl)).length) {
                                    var openLink = $('.trade-state-active[data-level="2"]', $('ul[data-parent="' + thisId + '"]', thisEl));
                                    var openId = openLink.attr('data-id');
                                    var openLevel = $('ul[data-parent="' + openId + '"]', thisEl);
                                } else {
                                    var openLink = $('[data-level="2"]:first', $('ul[data-parent="' + thisId + '"]', thisEl));
                                    var openId = openLink.attr('data-id');
                                    var openLevel = $('ul[data-parent="' + openId + '"]', thisEl);
                                    $('[data-level="2"]:first', $('ul[data-parent="' + thisId + '"]', thisEl)).addClass('trade-state-active');
                                    openLink.addClass('trade-state-active');
                                }
                                openLevel.removeClass('trade-state-close');
                            }

                            $('ul[data-level="' + thisLevel + '"]', thisEl).addClass('trade-state-close');
                            $(this).addClass('trade-state-active');

                            $('ul[data-parent="' + thisId + '"]', thisEl).removeClass('trade-state-close');

                            // ajax load materials
                            if (thisLevel == 2 && $(this).hasClass('trade-ajax-content-need-load') && typeof thisNid != undefined && typeof thisId != undefined) {
                                if (!$(this).hasClass('trade-ajax-content-load-process')) {
                                    var thisCont = $('ul[data-parent="' + thisId + '"]', thisEl);
                                    $(this).addClass('trade-ajax-content-load-process');
                                    tradeUpholsteryPickerLoadMaterials(thisCont, thisNid, thisId, $(this));
                                }
                            }
                        }

                        // set uho
                        thisEl.ready(function () {
                            tradeUpholsteryPickerInitUho(thisEl);
                        });

                        return false;
                    });

                    //set uho height
                    thisEl.ready(function () {
                        tradeUpholsteryPickerInitUho(thisEl);
                    });

                });

            };

            tradeUpholsteryPickerInit();

            $(document).bind('documentChange', function () {
                tradeUpholsteryPickerInit();
            });

        }
    };

})(jQuery);