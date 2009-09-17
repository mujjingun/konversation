/*
  This class represents a DCC transferlist model.
*/

/*
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

/*
  Copyright (C) 2009 Bernd Buschinski <b.buschinski@web.de>
*/

#include "transferlistmodel.h"

#include <QApplication>
#include <KCategorizedSortFilterProxyModel>
#include <klocalizedstring.h>
#include <KDebug>

namespace Konversation
{
    namespace DCC
    {
        QString TransferHeaderData::typeToName(int headertype)
        {
            switch (headertype)
            {
                case FileName:
                    return i18n("File");
                case PartnerNick:
                    return i18n("Partner");
                case OfferDate:
                    return i18n("Started At");
                case Progress:
                    return i18n("Progress");
                case Position:
                    return i18n("Position");
                case CurrentSpeed:
                    return i18n("Speed");
                case SenderAdress:
                    return i18n("Sender Address");
                case Status:
                    return i18n("Status");
                case TimeLeft:
                    return i18n("Remaining");
                case TypeIcon:
                    return i18n("Type");
                default:
                    return "";
            };
        }


        TransferProgressBarDelete::TransferProgressBarDelete (QObject *parent)
            : QStyledItemDelegate (parent)
        {
        }

        void TransferProgressBarDelete::paint (QPainter *painter,
                                               const QStyleOptionViewItem &option,
                                               const QModelIndex &index) const
        {
            if (index.isValid())
            {
                int type = index.data(TransferListModel::TransferDisplayType).toInt();
                if (type == TransferItemData::SendCategory ||
                    type == TransferItemData::ReceiveCategory ||
                    type == TransferItemData::SpaceRow)
                {
                    return;
                }
            }

             QStyleOptionProgressBar progressBarOption;
             progressBarOption.rect = option.rect;
             progressBarOption.minimum = 0;
             progressBarOption.maximum = 100;
             progressBarOption.progress = index.data().toInt();
             progressBarOption.text = QString::number(progressBarOption.progress) + '%';
             progressBarOption.textVisible = true;

             QApplication::style()->drawControl(QStyle::CE_ProgressBar,
                                                &progressBarOption, painter);
        }


        TransferListProxyModel::TransferListProxyModel (QObject *parent)
            : QSortFilterProxyModel (parent)
        {
        }

        bool TransferListProxyModel::lessThan (const QModelIndex &left,
                                               const QModelIndex &right) const
        {
            int leftType = left.data(TransferListModel::TransferDisplayType).toInt();
            int rightType = right.data(TransferListModel::TransferDisplayType).toInt();

            if (leftType == rightType)
            {
                if (left.data(TransferListModel::HeaderType) == TransferHeaderData::Position)
                {
                    return (left.data(TransferListModel::TransferProgress).toInt() <
                            right.data(TransferListModel::TransferProgress).toInt());
                }
                else if (left.data(TransferListModel::HeaderType) == TransferHeaderData::OfferDate)
                {
                    return (left.data(TransferListModel::TransferOfferDate).toDateTime() <
                            right.data(TransferListModel::TransferOfferDate).toDateTime());
                }
                return QSortFilterProxyModel::lessThan (left, right);
            }

            //visible order should always be
            //DCCReceiveCategory > DCCReceiveItem > SpacerRow > DCCSendCategory > DCCSendItem
#if (QT_VERSION < QT_VERSION_CHECK(4, 5, 0))
            if (m_sortOrder == Qt::AscendingOrder)
#else
            if (sortOrder() == Qt::AscendingOrder)
#endif
            {
                return !(leftType < rightType);
            }
            return (leftType < rightType);
        }

#if (QT_VERSION < QT_VERSION_CHECK(4, 5, 0))
        void TransferListProxyModel::sort(int column, Qt::SortOrder order)
        {
            m_sortOrder = order;
            QSortFilterProxyModel::sort(column,order);
        }
#endif

        TransferListModel::TransferListModel(QObject *parent)
            : QAbstractListModel(parent)
        {
        }

        void TransferListModel::append(const TransferItemData &item)
        {
            m_transferList.append(item);

            if (item.transfer)
            {
                connect (item.transfer, SIGNAL(statusChanged(Konversation::DCC::Transfer*,int,int)),
                         this, SLOT(transferStatusChanged(Konversation::DCC::Transfer*,int,int)));
            }
        }

        Qt::ItemFlags TransferListModel::flags (const QModelIndex &index) const
        {
            if (index.isValid())
            {
                int type = index.data(TransferDisplayType).toInt();
                if (type == TransferItemData::ReceiveCategory ||
                    type == TransferItemData::SendCategory ||
                    type == TransferItemData::SpaceRow)
                {
                    return Qt::NoItemFlags;
                }
            }
            return QAbstractItemModel::flags(index);
        }

        void TransferListModel::transferStatusChanged (Transfer *transfer, int/* oldstatus*/,
                                                       int/* newstatus*/)
        {
            for (int i = 0; i < m_transferList.count(); ++i)
            {
                const TransferItemData &item = m_transferList.at(i);
                if (transfer == item.transfer)
                {
                    emit dataChanged(createIndex(i, 0), createIndex(i, columnCount() - 1));
                    return;
                }
            }
        }

        void TransferListModel::appendHeader(TransferHeaderData data)
        {
            data.name = TransferHeaderData::typeToName(data.type);
            m_headerList.append(data);
        }

        QVariant TransferListModel::data (const QModelIndex &index, int role) const
        {
            if(!index.isValid() || index.row() >= m_transferList.count())
            {
                return QVariant();
            }

            if (role == TransferDisplayType)
            {
                return m_transferList[index.row()].displayType;
            }
            else if (role == KCategorizedSortFilterProxyModel::CategoryDisplayRole)
            {
                return displayTypeToString(m_transferList[index.row()].displayType);
            }
            else if (role == HeaderType)
            {
                return headerData(index.column(), Qt::Horizontal, role);
            }

            Transfer *transfer = 0;
            if (m_transferList[index.row()].displayType == TransferItemData::ReceiveItem ||
                m_transferList[index.row()].displayType == TransferItemData::SendItem)
            {
                transfer = m_transferList[index.row()].transfer;
            }
            else
            {
                //category item
                return QVariant();
            }

            switch (role)
            {
                case Qt::DisplayRole:
                {
                    int type = columnToHeaderType(index.column());
                    switch (type)
                    {
                        case TransferHeaderData::FileName:
                            return transfer->getFileName();
                        case TransferHeaderData::PartnerNick:
                            return transfer->getPartnerNick();
                        case TransferHeaderData::Progress:
                            return transfer->getProgress();
                        case TransferHeaderData::OfferDate:
                            return transfer->getTimeOffer().toString("hh:mm:ss");
                        case TransferHeaderData::Position:
                            return getPositionPrettyText(transfer->getTransferringPosition(),
                                                         transfer->getFileSize());
                        case TransferHeaderData::CurrentSpeed:
                            return getSpeedPrettyText(transfer->getCurrentSpeed());
                        case TransferHeaderData::SenderAdress:
                            return getSenderAddressPrettyText(transfer);
                        case TransferHeaderData::Status:
                            return getStatusText(transfer->getStatus(), transfer->getType());
                        case TransferHeaderData::TimeLeft:
                            return getTimeLeftPrettyText(transfer->getTimeLeft());

                        default:
                            kDebug() << "unknown columntype: " << type;
                        case TransferHeaderData::TypeIcon:
                            return QVariant();
                    };
                    break;
                }

                case TransferType:
                    return transfer->getType();
                case TransferStatus:
                    return transfer->getStatus();
                case TransferPointer:
                    return qVariantFromValue<QObject*>(transfer);
                case TransferProgress:
                    return transfer->getProgress();
                case TransferOfferDate:
                    return transfer->getTimeOffer();
                case Qt::DecorationRole:
                {
                    int type = columnToHeaderType(index.column());
                    switch (type)
                    {
                        case TransferHeaderData::Status:
                        {
                            QVariant decoration(QVariant::Pixmap);
                            decoration.setValue<QPixmap>(getStatusIcon(transfer->getStatus()));
                            return decoration;
                        }
                        case TransferHeaderData::TypeIcon:
                        {
                            QVariant decoration(QVariant::Pixmap);
                            decoration.setValue<QPixmap>(getTypeIcon(transfer->getType()));
                            return decoration;
                        }
                        default:
                            return QVariant();
                    }
                    break;
                }
                case Qt::ToolTipRole:
                {
                    int type = columnToHeaderType(index.column());
                    QString tooltip;
                    switch (type)
                    {
                        case TransferHeaderData::FileName:
                            tooltip = transfer->getFileName();
                            break;
                        case TransferHeaderData::PartnerNick:
                            tooltip = transfer->getPartnerNick();
                            break;
                        case TransferHeaderData::Progress:
                            tooltip = QString::number(transfer->getProgress()) + '%';
                            break;
                        case TransferHeaderData::OfferDate:
                            tooltip = transfer->getTimeOffer().toString("hh:mm:ss");
                            break;
                        case TransferHeaderData::Position:
                            tooltip = getPositionPrettyText(transfer->getTransferringPosition(),
                                                            transfer->getFileSize());
                            break;
                        case TransferHeaderData::CurrentSpeed:
                            tooltip = getSpeedPrettyText(transfer->getCurrentSpeed());
                            break;
                        case TransferHeaderData::SenderAdress:
                            tooltip = getSenderAddressPrettyText(transfer);
                            break;
                        case TransferHeaderData::Status:
                            tooltip = getStatusDescription(transfer->getStatus(), transfer->getType(), transfer->getStatusDetail());
                            break;
                        case TransferHeaderData::TimeLeft:
                            tooltip = getTimeLeftPrettyText(transfer->getTimeLeft());
                            break;
                        case TransferHeaderData::TypeIcon:
                            tooltip = displayTypeToString(m_transferList[index.row()].displayType);
                            break;
                        default:
                            kDebug() << "unknown columntype: " << type;
                            break;
                    };

                    if (tooltip.isEmpty())
                    {
                        return QVariant();
                    }
                    else
                    {
                        return "<qt>" + tooltip + "</qt>";
                    }
                }
                default:
                    return QVariant();
            }
        }

        QPixmap TransferListModel::getTypeIcon(Transfer::Type type) const
        {
            if (type == Transfer::Send)
            {
                return KIconLoader::global()->loadIcon("arrow-up", KIconLoader::Small);
            }
            else
            {
                return KIconLoader::global()->loadIcon("arrow-down", KIconLoader::Small);
            }
        }

        QPixmap TransferListModel::getStatusIcon(Transfer::Status status) const
        {
            QString icon;
            switch (status)
            {
                case Transfer::Queued:
                    icon = "media-playback-stop";
                    break;
                case Transfer::Preparing:
                case Transfer::WaitingRemote:
                case Transfer::Connecting:
                    icon = "network-disconnect";
                    break;
                case Transfer::Transferring:
                    icon = "media-playback-start";
                    break;
                case Transfer::Done:
                    icon = "dialog-ok";
                    break;
                case Transfer::Aborted:
                case Transfer::Failed:
                    icon = "process-stop";
                    break;
                default:
                break;
            }
            return KIconLoader::global()->loadIcon(icon, KIconLoader::Small);
        }

        QString TransferListModel::getSpeedPrettyText (transferspeed_t speed)
        {
            if (speed == Transfer::Calculating || speed == Transfer::InfiniteValue)
            {
                return QString("?");
            }
            else if (speed == Transfer::NotInTransfer)
            {
                return QString();
            }
            else
            {
                return i18n("%1/sec", KIO::convertSize((KIO::fileoffset_t)speed));
            }
        }

        QString TransferListModel::getPositionPrettyText(KIO::fileoffset_t position,
                                                         KIO::filesize_t filesize) const
        {
            return KIO::convertSize(position) + " / " + KIO::convertSize(filesize);
        }

        QString TransferListModel::getSenderAddressPrettyText(Transfer *transfer) const
        {
            if (transfer->getType() == Transfer::Send)
            {
                return QString("%1:%2").arg(transfer->getOwnIp()).arg(transfer->getOwnPort());
            }
            else
            {
                return QString("%1:%2").arg(transfer->getPartnerIp()).arg(transfer->getPartnerPort());
            }
        }

        QString TransferListModel::getTimeLeftPrettyText(int timeleft)
        {
            switch (timeleft)
            {
                case Transfer::InfiniteValue:
                    return QString('?');
                default:
                    return secToHMS(timeleft);
                case Transfer::NotInTransfer:
                    return "";
            }
        }

        QString TransferListModel::secToHMS(long sec)
        {
                int remSec = sec;
                int remHour = remSec / 3600;
                remSec -= remHour * 3600;
                int remMin = remSec / 60;
                remSec -= remMin * 60;

                // remHour can be more than 25, so we can't use QTime here.
                return QString("%1:%2:%3")
                    .arg(QString::number(remHour).rightJustified(2, '0', false))
                    .arg(QString::number(remMin).rightJustified(2, '0'))
                    .arg(QString::number(remSec).rightJustified(2, '0'));
        }

        QString TransferListModel::getStatusText(Transfer::Status status, Transfer::Type type)
        {
            switch (status)
            {
                case Transfer::Queued:
                    return i18n("Queued");
                case Transfer::Preparing:
                    return i18n("Preparing");
                case Transfer::WaitingRemote:
                    return i18n("Pending");
                case Transfer::Connecting:
                    return i18n("Connecting");
                case Transfer::Transferring:
                    switch (type)
                    {
                        case Transfer::Receive:
                            return i18n("Receiving");
                        case Transfer::Send:
                            return i18n("Sending");
                        default:
                            kDebug() << "unknown type: " << type;
                            return QString();
                    }
                case Transfer::Done:
                    return i18n("Done");
                case Transfer::Failed:
                    return i18n("Failed");
                case Transfer::Aborted:
                    return i18n("Aborted");
                default:
                    kDebug() << "unknown status: " << status;
                    return QString();
            }
        }

        QString TransferListModel::getStatusDescription(Transfer::Status status, Transfer::Type type, const QString& errorMessage) const
        {
            switch (status)
            {
                case Transfer::Queued:
                    return i18n("Queued - Transfer is waiting for you to accept or reject it");
                case Transfer::Preparing:
                    switch (type)
                    {
                        case Transfer::Receive:
                            return i18n("Preparing - Transfer is checking for resumable files");
                        case Transfer::Send:
                            return i18n("Preparing - Transfer is acquiring the data to send");
                        default:
                            kDebug() << "unknown type: " << type;
                            return QString();
                    }
                case Transfer::WaitingRemote:
                    return i18n("Pending - Transfer is waiting for the partner to accept or reject it");
                case Transfer::Connecting:
                    return i18n("Connecting - Transfer is connecting to the partner");
                case Transfer::Transferring:
                    switch (type)
                    {
                        case Transfer::Receive:
                            return i18n("Receiving - Transfer is receiving data from the partner");
                        case Transfer::Send:
                            return i18n("Sending - Transfer is sending data to partner");
                        default:
                            kDebug() << "unknown type: " << type;
                            return QString();
                    }
                case Transfer::Done:
                    return i18n("Done - Transfer has completed successfully");
                case Transfer::Failed:
                    if (errorMessage.isEmpty())
                    {
                        return i18n("Failed - Transfer failed with 'unknown reason'");
                    }
                    else
                    {
                        return i18n("Failed - Transfer failed with reason '%1'", errorMessage);
                    }
                case Transfer::Aborted:
                    return i18n("Aborted - Transfer was aborted by the User");
                default:
                    kDebug() << "unknown status: " << status;
                    return QString();
            }
        }

        int TransferListModel::columnToHeaderType (int column) const
        {
            return m_headerList.at(column).type;
        }

        int TransferListModel::columnCount (const QModelIndex &/*parent*/) const
        {
            return m_headerList.count();
        }

        QVariant TransferListModel::headerData (int section, Qt::Orientation orientation,
                                                int role) const
        {
            if(orientation == Qt::Vertical || section >= columnCount() || section < 0)
            {
                return QVariant();
            }

            switch (role)
            {
                case Qt::DisplayRole:
                    return m_headerList.at(section).name;
                case HeaderType:
                    return m_headerList.at(section).type;
                default:
                    return QVariant();
            }
        }

        int TransferListModel::rowCount (const QModelIndex &/*parent*/) const
        {
            return m_transferList.count();
        }

        bool TransferListModel::removeRow (int row, const QModelIndex &parent)
        {
            return removeRows(row, 1, parent);
        }

        bool TransferListModel::removeRows (int row, int count, const QModelIndex &parent)
        {
            if (count < 1 || row < 0 || (row + count) > rowCount(parent))
            {
                return false;
            }

            beginRemoveRows(parent, row, row + count - 1);

            bool blockSignal = signalsBlocked();
            blockSignals(true);

            for (int i = row + count - 1; i >= row; --i)
            {
                TransferItemData item = m_transferList.takeAt(i);
                //Category items don't have a transfer
                if (item.transfer)
                {
                    item.transfer->removedFromView();
                }
            }

            blockSignals(blockSignal);
            endRemoveRows();

            emit rowsPermanentlyRemoved (row, row + count - 1);

            return true;
        }

        int TransferListModel::itemCount (TransferItemData::ItemDisplayType displaytype) const
        {
            int count = 0;
            foreach (const TransferItemData &item, m_transferList)
            {
                if (item.displayType == displaytype)
                {
                    ++count;
                }
            }
            return count;
        }

        QString TransferListModel::displayTypeToString (int type) const
        {
            if (type == TransferItemData::ReceiveCategory)
            {
                return i18n("Incoming Transfers");
            }
            else
            {
                return i18n("Outgoing Transfers");
            }
        }

    }
}