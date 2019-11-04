#ifndef __COUNTED_REF_TO__
    #define __COUNTED_REF_TO__

    // -- GLOBAL

    // .. TYPES

    template <class _OBJECT_>
    class COUNTED_REF_TO_
    {
        // -- PUBLIC

    public :

        typedef _OBJECT_ OBJECT;

        // .. CONSTRUCTORS

        COUNTED_REF_TO_(
            void
            ):
            Object( NULL )
        {
        }

        // ~~

        COUNTED_REF_TO_(
            const COUNTED_REF_TO_ & other
            ):
            Object( NULL )
        {
            SetObject( other.Object );
        }

        // ~~

        COUNTED_REF_TO_(
            const _OBJECT_ * object
            ):
            Object( NULL )
        {
            SetObject( object );
        }

        // ~~

        ~COUNTED_REF_TO_(
            void
            )
        {
            SetNull();
        }

        // .. OPERATORS

        COUNTED_REF_TO_ & operator=(
            const COUNTED_REF_TO_ & other
            )
        {
            SetObject( other.Object );

            return *this;
        }

        // ~~

        COUNTED_REF_TO_ & operator=(
            const _OBJECT_ * object
            )
        {
            SetObject( object );

            return *this;
        }

        // ~~

        operator _OBJECT_ *( 
            void
            ) const
        {
            return Object;
        }

        // ~~

        _OBJECT_ & operator*( 
            void
            ) const
        {
            return *Object;
        }

        // ~~

        _OBJECT_ * operator->( 
            void
            ) const
        {
            return Object;
        }

        // ~~

        operator _OBJECT_ &(
            void
            )
        {
            return Object;
        }

        // ~~

        operator const _OBJECT_ &(
            void
            ) const
        {
            return Object;
        }

        // .. OPERATIONS

        void SetNull(
            void
            )
        {
            if ( Object != NULL )
            {
                Object->RemoveRef();
                Object = NULL;
            }
        }

        // ~~

        void SetObject(
            const _OBJECT_ * object
            )
        {
            if ( Object != object )
            {
                if ( object != NULL )
                {
                    const_cast<_OBJECT_*>( object )->AddRef();
                }

                // inlined SetNull();
                if ( Object != NULL )
                {
                    Object->RemoveRef();
                }

                Object = const_cast<_OBJECT_*>( object );
            }
        }


    private:

        _OBJECT_
            * Object;
    };

    template <class _OBJECT_>
    inline void META_SCRIPTABLE_PushValue(
        const COUNTED_REF_TO_<_OBJECT_> & value,
        lua_State * lua_state,
        const COUNTED_REF_TO_<_OBJECT_> * dummy
        )
    {
        META_SCRIPTABLE_PushValue(*value, lua_state, ( _OBJECT_* ) 0 );
    }

    // ~~

    template <class _OBJECT_>
    inline COUNTED_REF_TO_<_OBJECT_> META_SCRIPTABLE_GetValue(
        lua_State * lua_state,
        int value_index,
        const COUNTED_REF_TO_<_OBJECT_> * dummy
        )
    {
        return &META_SCRIPTABLE_GetValue( lua_state, value_index, ( _OBJECT_* ) 0 );
    }
             
#endif
