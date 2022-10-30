#ifndef EITHER_H
#define EITHER_H

#include <QVariant>

#include <wall_e/src/models/either.h>

class Either {
    Q_GADGET
    Q_PROPERTY(bool defined READ defined)
    Q_PROPERTY(QString left READ left)
    Q_PROPERTY(QVariant right READ right)

    bool m_defined;
    QString m_left;
    QVariant m_right;

    Either(
            bool defined,
            const QString& left,
            const QVariant& right
            )
        : m_defined(defined),
          m_left(left),
          m_right(right) {}
public:
    Either() : Either(false, {}, {}) {}
    inline static Either newLeft(const QString& err) { return Either(false, err, QVariant()); }
    inline static Either newRight(const QVariant& val) { return Either(true, QString(), val); }

    template<typename L, typename R>
    inline static Either fromWallEEither(const wall_e::either<L, R>& e) {
        if(e) {
            return Either(true, QString(), QVariant::fromValue(e.right_value()));
        } else {
            return Either(false, QString::fromStdString(e.left_value()), QVariant());
        }
    }

    inline bool defined() const { return m_defined; }
    inline QString left() const { return m_left; }
    inline QVariant right() const { return m_right; }

    bool operator==(const Either& other) const;
};

Q_DECLARE_METATYPE(Either)

#endif // EITHER_H
