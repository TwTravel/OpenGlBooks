function Initialize()

    local player = PLAYER_ENTITY:new( 100 )
    player:SetPosition( VECTOR_3:new( 1, 2, 3 ) )
    WORLD:AddEntity( player, "player" )
    DUMMY_OF_ENTITY:new( player )
end

function PrintObjectHealth( entity_name )
    
    -- singleton or static call
    
    entity = WORLD:GetEntity( entity_name );
    
    print( "lua: entity" .. entity_name .. " GetHealth = " .. entity:GetHealth() );
    local position = entity:GetPosition();
    print( "position : " .. position:GetX() .. ", " .. position:GetY() .. ", " .. position:GetZ() )
    
end