#include "model.hxx"
#include <catch.hxx>

// We'll use this same game config throughout.
Game_config const config;

TEST_CASE("ball moving freely")
{
    Model m(config);

    // Get rid of all the bricks and make the ball move slowly upward so
    // we don't hit anything:
    m.bricks.clear();
    m.ball.velocity = {8, -24};
    m.ball.live = true;

    // Make a copy of the ball's position, which we'll use to compute
    // where we expect the ball to go.
    Position expected_center = m.ball.center;

    // If the model advances by 0.5 s then we expect the center to move
    // accordingly:
    m.on_frame(0.5);
    expected_center += 0.5 * m.ball.velocity;
    CHECK(m.ball.center == expected_center);

    // Again, but only 0.25 s this time.
    m.on_frame(0.25);
    expected_center += 0.25 * m.ball.velocity;
    CHECK(m.ball.center == expected_center);

    // Again, but 0.125 s this time.
    m.on_frame(0.125);
    expected_center += 0.125 * m.ball.velocity;
    CHECK(m.ball.center == expected_center);
}

TEST_CASE("destroy one brick")
{
    Model m(config);

    // Let's run at 1 fps.
    double const dt = 1;

    // We're going to stub the model's random number generator so it
    // always returns 14:
    float const boost = 14;

    // Stub it:
    m.random_boost_source.stub_with(boost);

    // Make there be only one brick:
    m.bricks.clear();
    m.bricks.push_back({250, 200, 100, 20});

    // The ball is headed toward the brick and should hit it in
    // the 4th frame if we simulate 1 frame per second:
    m.ball.live = true;
    m.ball.center = {300, 400};
    m.ball.velocity = {0, -50};

    // Make a copy of the ball, which we'll use to computer what we
    // expect the ball to do:
    Ball expected_ball(m.ball);

    // Simulate the passage of 1 frame (1 s):
    m.on_frame(dt);
    expected_ball = expected_ball.next(dt);
    CHECK(m.ball == expected_ball);
    CHECK(m.bricks.size() == 1);

    // Simulate the passage of another frame:
    m.on_frame(dt);
    expected_ball = expected_ball.next(dt);
    CHECK(m.ball == expected_ball);
    CHECK(m.bricks.size() == 1);

    // Simulate the passage of another frame:
    m.on_frame(dt);
    expected_ball = expected_ball.next(dt);
    CHECK(m.ball == expected_ball);
    CHECK(m.bricks.size() == 1);

    // Simulate the passage of another frame, in which the ball
    // destroys a brick. The model should reflect and boost the ball
    // before moving it, so we need to do the same with our expected
    // ball.
    m.on_frame(dt);
    expected_ball.velocity.height *= -1;
    expected_ball.velocity.width += boost;
    expected_ball = expected_ball.next(dt);
    CHECK(m.ball == expected_ball);
    CHECK(m.bricks.empty());
}

TEST_CASE("bounce off paddle")
{
    Model m(config);

    // No bricks, please.
    m.bricks.clear();

    // Let's run at 8 fps.
    double const dt = 0.125;

    // The ball is headed toward the paddle at 50 px/s from 75 px away,
    // so it should it it in 1.5 s. If we simulate 8 fps then it should
    // hit in the 12th frame.
    m.paddle = {50, 400, 600, 50};
    m.ball.live = true;
    m.ball.velocity = {0, 50};
    m.ball.center.x = 300;
    m.ball.center.y = m.paddle.y - 75 - m.ball.radius / 2;

    // Make a copy of the ball for computing what we expect it to do.
    Ball expected_ball(m.ball);

    // Simulate 11 frames (1.375 s).
    for (int i = 0; i < 11; ++i) {
        // run both the expected ball and the model for a frame
        // then check that they agree still
        expected_ball = expected_ball.next(dt);
        m.on_frame(dt);
        CHECK(m.ball == expected_ball);
    }

    // Simulate one frame in which the ball finally hits the paddle,
    // which means that it reflects vertically.
    expected_ball.velocity.height *= -1;
    expected_ball = expected_ball.next(dt);
    m.on_frame(dt);
    CHECK(m.ball == expected_ball);

    // Simulate 8 more frames (1 s), now with the ball headed away from
    // the paddle.
    for (int i = 0; i < 8; ++i) {
        expected_ball = expected_ball.next(dt);
        m.on_frame(dt);
        CHECK(m.ball == expected_ball);
    }
}

//
// "WRITE MORE TESTS!!!" is never bad advice
//
TEST_CASE("Off top")
{
    Model m(config);

    // No bricks, please.
    m.bricks.clear();

    // Let's run at 1 fps.
    double const dt = 1;

    // The ball is headed toward the paddle at 24 px/s from 72 px away,
    // so it should hit it in 3 s. If we simulate 8 fps then it should
    // hit in the 3rd frame.
    m.ball.live = true;
    m.ball.velocity = {-50, 0};
    m.ball.center.x = 300;
    m.ball.center.y = 300;

    // Make a copy of the ball for computing what we expect it to do.
    Ball expected_ball(m.ball);

    // Simulate frames
    for (int i = 0; i < 5; ++i) {
        // run both the expected ball and the model for a frame
        // then check that they agree still
        expected_ball = expected_ball.next(dt);
        m.on_frame(dt);
        CHECK(m.ball == expected_ball);
    }

    // Simulate one frame in which the ball finally hits the paddle,
    // which means that it reflects vertically.
    expected_ball.velocity.width *= -1;
    expected_ball = expected_ball.next(dt);
    m.on_frame(dt);
    CHECK(m.ball == expected_ball);

    // Simulate 8 more frames (1 s), now with the ball headed away from
    // the paddle.
    for (int i = 0; i < 5; ++i) {
        expected_ball = expected_ball.next(dt);
        m.on_frame(dt);
        CHECK(m.ball == expected_ball);
    }

}

TEST_CASE("Off side and top")
{
    Model m(config);

    // No bricks, please.
    m.bricks.clear();

    // Let's run at 1 fps.
    double const dt = 1;

    // The ball is headed toward the paddle at 24 px/s from 72 px away,
    // so it should hit it in 3 s. If we simulate 8 fps then it should
    // hit in the 3rd frame.
    m.ball.live = true;
    m.ball.velocity = {-50, -50};
    m.ball.center.x = 300;
    m.ball.center.y = 300;

    // Make a copy of the ball for computing what we expect it to do.
    Ball expected_ball(m.ball);

    // Simulate frames
    for (int i = 0; i < 5; ++i) {
        // run both the expected ball and the model for a frame
        // then check that they agree still
        expected_ball = expected_ball.next(dt);
        m.on_frame(dt);
        CHECK(m.ball == expected_ball);
    }

    // Simulate one frame in which the ball finally hits the paddle,
    // which means that it reflects vertically.
    expected_ball.velocity.width *= -1;
    expected_ball.velocity.height *= -1;
    expected_ball = expected_ball.next(dt);
    m.on_frame(dt);
    CHECK(m.ball == expected_ball);

    // Simulate 8 more frames (1 s), now with the ball headed away from
    // the paddle.
    for (int i = 0; i < 5; ++i) {
        expected_ball = expected_ball.next(dt);
        m.on_frame(dt);
        CHECK(m.ball == expected_ball);
    }

}

TEST_CASE("Paddle Moves with Ball")
{
    Model m(config);

    m.ball.live = false;
    Position expected_center = m.ball.center;
    m.paddle_to(100);
    expected_center.x = 150;
    CHECK(m.ball.center == expected_center);

    m.paddle_to(50);
    expected_center.x = 100;
    CHECK(m.ball.center == expected_center);

    m.paddle_to(500);
    expected_center.x = 550;
    CHECK(m.ball.center == expected_center);

}